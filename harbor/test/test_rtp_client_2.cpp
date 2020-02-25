#include <stdio.h>  
#include <libavformat/avformat.h>  
#include <libavutil/mathematics.h>  
#include <libavutil/time.h> 
#include <libavcodec/avcodec.h>


int main(int argc, char* argv[])  
{  
    AVOutputFormat *ofmt = NULL;  
    //输入对应一个AVFormatContext，输出对应一个AVFormatContext  
    //（Input AVFormatContext and Output AVFormatContext）  
    AVFormatContext *ifmt_ctx_a = NULL, *ifmt_ctx_v = NULL, *ifmt_ctx, 
                    *ofmt_ctx[2];  
    AVPacket pkt,pkt1;
    const char *in_filename_a, *in_filename_v, 
                *out_filename_a, *out_filename_v;  
    int ret, i;  
    int videoindex=-1, audioindex=-1;  
    int vframe_index=0, aframe_index = 0,  index = 0; 
    int64_t start_time=0;  
    int64_t cur_pts_v=0, cur_pts_a=0;  
    in_filename_v = "test.h264";//输入视频文件
    in_filename_a = "test.aac";//输入音频文件

    out_filename_v = "rtp://192.168.1.121:6666";//视频端口
    out_filename_a = "rtp://192.168.1.121:6668";//音频端口                                                                   
    av_register_all();  
    //Network  
    avformat_network_init();  
    //视频输入结构体初始化 
    if ((ret = avformat_open_input(&ifmt_ctx_v, in_filename_v, 0, 0)) < 0) {  
        printf( "Could not open input file.");  
        goto end;  
    }  
    if ((ret = avformat_find_stream_info(ifmt_ctx_v, 0)) < 0) {  
        printf( "Failed to retrieve input stream information");  
        goto end;  
    }  

    //音频输入结构体初始化
    if ((ret = avformat_open_input(&ifmt_ctx_a, in_filename_a, 0, 0)) < 0) {  
        printf( "Could not open input file.");  
        goto end;  
    }  
    if ((ret = avformat_find_stream_info(ifmt_ctx_a, 0)) < 0) {  
        printf( "Failed to retrieve input stream information");  
        goto end;  
    }  


    /**********打印输入文件信息**********/
    av_dump_format(ifmt_ctx_v, 0, in_filename_v, 0);    
    av_dump_format(ifmt_ctx_a, 1, in_filename_a, 0); 
    /************************************/


    //rtp输出结构体初始化
    avformat_alloc_output_context2(ofmt_ctx, NULL, "rtp_mpegts", NULL);
    avformat_alloc_output_context2(ofmt_ctx+1, NULL, "rtp_mpegts", NULL);


    if (!ofmt_ctx) {  
        printf( "Could not create output context\n");  
        ret = AVERROR_UNKNOWN;  
        goto end;  
    }  

    //视频部分
    for (i = 0; i < ifmt_ctx_v->nb_streams; i++) {  
        //根据输入流创建输出流
        AVStream *in_stream = ifmt_ctx_v->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx[0], in_stream->codec->codec);  
        if (!out_stream) {  
            printf( "Failed allocating output stream\n");  
            ret = AVERROR_UNKNOWN;  
            goto end;  
        }    
        //复制AVCodecContext的设置
        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);  
        if (ret < 0) {  
            printf( "Failed to copy context from input to output stream codec context\n");  
            goto end;  
        }  
        out_stream->codec->codec_tag = 0;  
        if (ofmt_ctx[0]->oformat->flags & AVFMT_GLOBALHEADER)  
            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;  
    }  
    //音频部分
    for (i = 0; i < ifmt_ctx_a->nb_streams; i++) {  
        //根据输入流创建输出流
        AVStream *in_stream = ifmt_ctx_a->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx[1], in_stream->codec->codec);  
        if (!out_stream) {  
            printf( "Failed allocating output stream\n");  
            ret = AVERROR_UNKNOWN;  
            goto end;  
        }    
        //复制AVCodecContext的设置 
        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);  
        if (ret < 0) {  
            printf( "Failed to copy context from input to output stream codec context\n");  
            goto end;  
        }  
        out_stream->codec->codec_tag = 0;  
        if (ofmt_ctx[1]->oformat->flags & AVFMT_GLOBALHEADER)  
            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;  
    }  

    //打印输出的信息格式
    av_dump_format(ofmt_ctx[0], 0, out_filename_v, 1); 
    av_dump_format(ofmt_ctx[1], 1, out_filename_a, 1);

    //打开输出URL（Open output URL）  
    if (!((*ofmt_ctx)->oformat->flags & AVFMT_NOFILE)) {  
        ret = avio_open(&(ofmt_ctx[0])->pb, out_filename_v, AVIO_FLAG_WRITE);  
        if (ret < 0) {  
            printf( "Could not open output URL '%s'", out_filename_v);  
            goto end;  
        }
    }  
    if (!((*ofmt_ctx)->oformat->flags & AVFMT_NOFILE)) {  
        ret = avio_open(&(ofmt_ctx[1])->pb, out_filename_a, AVIO_FLAG_WRITE);  
        if (ret < 0) {  
            printf( "Could not open output URL '%s'", out_filename_a);  
            goto end;  
        }
    }  

    //写文件头（Write file header）  
    ret= avformat_write_header(ofmt_ctx[0], NULL);
    ret= avformat_write_header(ofmt_ctx[1], NULL);

    if (ret < 0) {  
        printf( "Error occurred when opening output URL\n");  
        goto end;  
    } 

    start_time=av_gettime();  
    while (1) {  
        AVStream *in_stream, *out_stream;  
        videoindex = 0;
        audioindex = 0;

        //Get an AVPacket  
        if(av_compare_ts(cur_pts_v,ifmt_ctx_v->streams[0]->time_base,cur_pts_a,ifmt_ctx_a->streams[0]->time_base) <= 0){  
            ifmt_ctx=ifmt_ctx_v;
            index = 0;
            if(av_read_frame(ifmt_ctx, &pkt) >= 0){  
                do{  
                    in_stream  = ifmt_ctx->streams[0];  
                    out_stream = ofmt_ctx[index]->streams[0];  

                    if(pkt.stream_index==index){  

                        AVRational time_base = in_stream->time_base;//{ 1, 1000 };  
                        AVRational r_framerate1 = in_stream->r_frame_rate;
                        AVRational time_base_q = { 1, AV_TIME_BASE };  
                        //Duration between 2 frames (us)  
                        int64_t calc_duration = (double)(AV_TIME_BASE) / av_q2d(r_framerate1);  //内部时间戳 
                        pkt.pts = av_rescale_q(vframe_index*calc_duration, time_base_q, time_base);
                        pkt.dts = pkt.pts; 
                        pkt.duration = av_rescale_q(calc_duration, time_base_q, time_base);
                        pkt.pos = -1;
                        vframe_index++;  

                        cur_pts_v=pkt.pts;  
                        //发送延时Important:Delay
                        {
                            AVRational time_base=ifmt_ctx->streams[index]->time_base;
                            AVRational time_base_q={1,AV_TIME_BASE};  
                            int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);  
                            int64_t now_time = av_gettime() - start_time;  
                            if (pts_time > now_time)  
                                av_usleep(pts_time - now_time);
                        }

                        break;  
                    }  
                }while(av_read_frame(ifmt_ctx, &pkt) >= 0);  
            }
            else{  
                break;  
            }  
        }
        else{  
            ifmt_ctx=ifmt_ctx_a;  
            index = 1;  
            if(av_read_frame(ifmt_ctx, &pkt) >= 0){  
                do{  
                    in_stream  = ifmt_ctx->streams[0];  
                    out_stream = ofmt_ctx[index]->streams[0];  

                    if(pkt.stream_index==0){  
                        //FIX：No PTS  
                        //Simple Write PTS  
                        AVRational time_base1=in_stream->time_base; 
                        AVRational time_base_q = { 1, AV_TIME_BASE }; 
                        double frame_size = (double)in_stream->codec->frame_size;
                        double sample_rate = (double)in_stream->codec->sample_rate;
                        //Duration between 2 frames (us)  
                        int64_t calc_duration=(double)(AV_TIME_BASE) * (frame_size/sample_rate);

                        pkt.pts = av_rescale_q(aframe_index*calc_duration, time_base_q, time_base1);
                        pkt.dts = pkt.pts; 
                        pkt.duration = av_rescale_q(calc_duration, time_base_q, time_base1);
                        pkt.pos = -1;

                        aframe_index++;  
                        cur_pts_a=pkt.pts;  
                        break;  
                    }  
                }while(av_read_frame(ifmt_ctx, &pkt) >= 0);  
            }else{  
                break;  
            }  

        } 

        //Convert PTS/DTS  
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));  
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));  
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);  
        pkt.pos = -1;  

        if(index == 0)
            printf("Write video Packet. size:%5d\tpts:%lld\n",pkt.size,pkt.pts);  
        if(index == 1)
            printf("Write audio Packet. size:%5d\tpts:%lld\n",pkt.size,pkt.pts);  
        if (av_interleaved_write_frame(ofmt_ctx[index], &pkt) < 0) {  
            printf( "Error muxing packet\n");  
            break;  
        }  
        av_free_packet(&pkt);  
    }
    //写文件尾（Write file trailer）  


    av_write_trailer(ofmt_ctx[0]);
    av_write_trailer(ofmt_ctx[1]);


end:  
    avformat_close_input(&ifmt_ctx_v);
    avformat_close_input(&ifmt_ctx_a);
    /* close output */  
    if (ofmt_ctx[0] && !(ofmt_ctx[0]->oformat->flags & AVFMT_NOFILE))  
        avio_close((ofmt_ctx[0])->pb);  
        avformat_free_context(ofmt_ctx[0]);  
    if (ofmt_ctx[1] && !(ofmt_ctx[1]->oformat->flags & AVFMT_NOFILE))  
        avio_close((ofmt_ctx[1])->pb);  
        avformat_free_context(ofmt_ctx[1]);  
    if (ret < 0 && ret != AVERROR_EOF) {  
        printf( "Error occurred.\n");  
        return -1;  
    }  
    return 0;  
} 
