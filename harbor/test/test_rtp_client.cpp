extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/time.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavdevice/avdevice.h"
#include "libavutil/audio_fifo.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavutil/imgutils.h"
#include "libavfilter/avfilter.h"
#include "libavutil/audio_fifo.h"
#include "libavutil/avutil.h"
#include "libavutil/frame.h"
#include "libavutil/samplefmt.h"
#include "libavformat/avio.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libavutil/opt.h"
#include <stdio.h>
}
#include "jrtplib3/rtcpapppacket.h"
#include "jrtplib3/rtpsession.h"
#include "jrtplib3/rtppacket.h"
#include "jrtplib3/rtpudpv4transmitter.h"
#include "jrtplib3/rtpipv4address.h"
#include "jrtplib3/rtpsessionparams.h"
#include "jrtplib3/rtperrors.h"
#include "jrtplib3/rtpsourcedata.h"
#include <jthread/jthread.h>
#include <iostream>

#define	SSRC		10001
#define PORT_BASE	11992
#define MAX_PACKET_LENGTH	1360

int main (int argc, char *argv[]) 
{
    if (argc == 1) 
    {
        printf("usage: pls input filename\n");
        return 0;
    }
    char *filename = argv[1];
    
    int status = -1;
    jrtplib::RTPSession session;
    jrtplib::RTPUDPv4TransmissionParams transparams;
    jrtplib::RTPSessionParams sessionparams;
    sessionparams.SetOwnTimestampUnit(1.0/90000.0);
    sessionparams.SetAcceptOwnPackets(true); 
    sessionparams.SetUsePredefinedSSRC(true);
    sessionparams.SetPredefinedSSRC(SSRC);

    transparams.SetPortbase(PORT_BASE);
    status = session.Create(sessionparams, &transparams);

    if (status < 0) 
    {
        std::cout << "ERROR:" << jrtplib::RTPGetErrorString(status) << std::endl;
    }

    std::string serverip_str = "127.0.0.1";
    int32_t dest_port = 9866;
    uint32_t dest_ip = inet_addr(serverip_str.c_str());
    dest_ip = ntohl(dest_ip);

    jrtplib::RTPIPv4Address addr(dest_ip,dest_port);
    status = session.AddDestination(addr);

    if (status < 0) 
    {
        std::cout << "ERROR:" << jrtplib::RTPGetErrorString(status) << std::endl;
    }
    session.SetDefaultPayloadType(96);//设置传输类型
    session.SetDefaultMark(true);         //设置位
    session.SetTimestampUnit(1.0/90000.0); //设置采样间隔
    session.SetDefaultTimestampIncrement(3600);//设置时间戳增加间隔 

    //av_register_all();
    avdevice_register_all();
    avformat_network_init();

    AVFormatContext *formatContext = nullptr;
    formatContext = avformat_alloc_context();

    AVDictionary *videoOptions;
    int nRet = avformat_open_input(&formatContext, filename, 0, &videoOptions);
    if (nRet < 0) 
    {
        printf("Open file error: %s\n", filename);
        return -1;
    }

    if (avformat_find_stream_info(formatContext, &videoOptions) < 0) 
    {
        printf("Can't find video stream\n");
        return -1;
    }

    int duration = formatContext->duration / AV_TIME_BASE;
    printf("file duration = %d\n", duration);
    uint8_t nStream = formatContext->nb_streams;

    int streamId = -1;
    AVCodecContext *codecContext = nullptr;

    for (int i = 0; i < nStream; i++) 
    {
        //codecContext = formatContext->streams[i]->codec;
        codecContext = avcodec_alloc_context3(NULL);
        avcodec_parameters_to_context(codecContext, formatContext->streams[i]->codecpar);

        if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO) 
        {
            streamId = i;
            AVCodec *vCodec = avcodec_find_decoder(codecContext->codec_id);
            if (vCodec == nullptr) 
            {
                perror("can't find codec");
                continue;
            }
            if(avcodec_open2(codecContext, vCodec, 0) < 0) 
            {
                perror("avcodec open error");
                continue;
            }
            break;
        }
    }
    printf("get width = %d, get height = %d, fmt = %d\n", codecContext->width, codecContext->height, codecContext->pix_fmt);
    
    AVDictionary *outVideoOptions = nullptr;
    AVCodecContext *outCodecCtx = nullptr;

    AVCodec *outAVCodec = avcodec_find_encoder(AV_CODEC_ID_H264);

    if (!outAVCodec) 
    {
        std::cout << "Have not found h264 codec" << std::endl; 
        return 1;
    }
    outCodecCtx = avcodec_alloc_context3(outAVCodec);
    if (!outCodecCtx) 
    {
        return 1;
    }
    outCodecCtx->codec_id = AV_CODEC_ID_H264;
    outCodecCtx ->codec_type = AVMEDIA_TYPE_VIDEO;
    outCodecCtx->pix_fmt  = AV_PIX_FMT_YUV420P;
    outCodecCtx->bit_rate = codecContext->bit_rate; // 400000
    //outCodecCtx->rc_max_rate = 400000;
    //outCodecCtx->rc_min_rate = 400000;
    outCodecCtx->width = codecContext->width;
    outCodecCtx->height = codecContext->height;
    outCodecCtx->gop_size = 250;//3
    outCodecCtx->max_b_frames = 0;
    AVRational rat = {1,25};
    //AVRational rat = {1,15};
    outCodecCtx->time_base = rat;
    outCodecCtx->qmax = 51;
    outCodecCtx->qmin = 10;

    if (codecContext->codec_id == AV_CODEC_ID_H264)
    {
        av_dict_set(&outVideoOptions,"tune", "zerolatency", 0);
        av_dict_set(&outVideoOptions,"preset", "veryslow", 0);
	//av_opt_set(c->priv_data, "superfast", "zerolatency", 0);   //对编码速度有影响
	//av_opt_set(c->priv_data, "preset", "slow", 0);          //对编码速度有影响
	//av_dict_set(&outVideoOptions, "preset", "ultrafast", 0);     //对编码速度有影响
	//av_opt_set(c->priv_data, "tune","stillimage,fastdecode,zerolatency",0);
	//av_opt_set(c->priv_data, "x264opts","crf=26:vbv-maxrate=728:vbv-bufsize=364:keyint=25",0);
    }
    int value = 0;
    value = avcodec_open2(outCodecCtx, outAVCodec, &outVideoOptions);
    if(value < 0) 
    {
        std::cout << "avcodec open2 error" << std::endl;
        return 1;
    }

    if (value < 0) 
    {
        return 1;
    } 

    AVFrame *h264Frame = av_frame_alloc();
    h264Frame->format = outCodecCtx->pix_fmt;
    h264Frame->width = outCodecCtx->width;
    h264Frame->height = outCodecCtx->height;
    h264Frame->pts = 0;

    int h264_outbuf_size;
    int nbytes = av_image_get_buffer_size(outCodecCtx->pix_fmt, outCodecCtx->width, outCodecCtx->height, 1);
    //int nbytes = avpicture_get_size(outCodecCtx->pix_fmt, outCodecCtx->width, outCodecCtx->height);
    uint8_t *h264_outbuf = (uint8_t*)av_malloc(nbytes);
    av_image_fill_arrays(h264Frame->data, h264Frame->linesize, h264_outbuf, AV_PIX_FMT_YUV420P, outCodecCtx->width, outCodecCtx->height, 1);

    SwsContext* swsCtx_= sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt, outCodecCtx->width, outCodecCtx->height, outCodecCtx->pix_fmt, SWS_BICUBIC, nullptr, nullptr, nullptr);

    AVPacket h264Packet;

    AVFrame *vframe = av_frame_alloc();
    AVPacket *packet = (AVPacket *) malloc(sizeof(AVPacket));
    int y_size = codecContext->width * codecContext->height;
    //packet 缓存每一帧
    av_new_packet(packet, y_size);
    int ret, got_picture, got_h264_pic;
    int vframe_index = 0;

    while (1) 
    {
        if(av_read_frame(formatContext, packet) >= 0) 
        {
            if (packet->stream_index == streamId) 
            {
                AVStream *video_st = formatContext->streams[streamId];
                //ret = avcodec_decode_video2(codecContext, vframe, &got_picture, packet);
                ret = avcodec_send_packet(codecContext, packet);
                if (ret != 0) 
                {
                    continue;
                }
                while(avcodec_receive_frame(codecContext, vframe) == 0) 
                {
                    sws_scale(swsCtx_, vframe->data, vframe->linesize, 0, codecContext->height, h264Frame->data, h264Frame->linesize);
                    av_init_packet(&h264Packet);
                    h264Packet.data = nullptr;
                    h264Packet.size = 0;
                    h264Packet.pts = AV_NOPTS_VALUE;
    		    h264Packet.dts = AV_NOPTS_VALUE;  
                    //h264Packet.pts = packet->pts;
    		    //h264Packet.dts = packet->dts;  
           
                    //h264Frame->pts = vframe->pkt_pts;
    
                    //avcodec_encode_video2(outCodecCtx, &h264Packet, h264Frame, &got_h264_pic);
                    //avcodec_encode_video2(outCodecCtx, &h264Packet, vframe, &got_h264_pic);
                    int ret = avcodec_send_frame(outCodecCtx, vframe);
                    if (ret < 0) 
                    {
                        return 0;
                    }
                    avcodec_receive_packet(outCodecCtx, &h264Packet);
                    
                    if(true) 
                    {
                        if(h264Packet.pts != AV_NOPTS_VALUE) 
                        {
                            h264Packet.pts = av_rescale_q(h264Packet.pts, video_st->time_base, video_st->time_base);
                        }
                        if(h264Packet.dts != AV_NOPTS_VALUE) 
                        {
                            h264Packet.dts = av_rescale_q(h264Packet.dts, video_st->time_base, video_st->time_base);
                        }
                        h264Packet.pts = packet->pts;
                        h264Packet.dts = packet->pts; 
                        /*
                        AVRational time_base = video_st->time_base;
                        AVRational r_framerate1 = video_st->r_frame_rate;
                        AVRational time_base_q = {1, AV_TIME_BASE};
                        int64_t calc_duration = (double)(AV_TIME_BASE) / av_q2d(r_framerate1);
                        h264Packet.pts = av_rescale_q(vframe_index * calc_duration, time_base_q, time_base);
                        h264Packet.dts = h264Packet.pts;
                        h264Packet.duration = av_rescale_q(calc_duration, time_base_q, time_base);
                        h264Packet.pos = -1;
                        vframe_index++;
                        */
    		    //h264Packet.pts = h264Frame->pts;
    		        //h264Packet.data[h264Packet.size] = 0x01;
                        
                        unsigned char *sendData = (unsigned char *)h264Packet.data;
                        int sendlen = h264Packet.size;
                        if (sendlen < MAX_PACKET_LENGTH) 
                        {
                            session.SetDefaultMark(true);
                            unsigned char sendbuf[1430] = {0};
                            memcpy(sendbuf, sendData + 4, sendlen - 4);
                            status = session.SendPacket(sendbuf, sendlen - 4);
                            if (status < 0) 
                            {
                                std::cout << "ERROR:" << jrtplib::RTPGetErrorString(status) << std::endl;
                            }
                        } 
                        else 
                        {
                            unsigned char naltype = sendData[4];
                            unsigned char fu_indicator = (naltype & 0x60) | 28;


                            session.SetDefaultMark(false);
                            int k = 0, l = 0, t = 0;
                            k = sendlen / MAX_PACKET_LENGTH;
                            l = sendlen % MAX_PACKET_LENGTH;
    
                            while ((t < k) || ((t == k) && (l > 0))) 
                            {
                                unsigned char sendbuf[1430] = {0};
                                uint32_t sending_len = 0; 
                                unsigned char fu_type = naltype & 0x1f;

                                if ((t == 0) || ((t < k) && (t != 0))) 
                                {
                                    session.SetDefaultMark(false);
                                    if (t == 0) 
                                    {
                                        fu_type |= 0x80;
                                        memcpy(sendbuf, &fu_indicator, 1);
                                        memcpy(sendbuf + 1, &fu_type, 1);
                                        memcpy(sendbuf + 2, sendData + 5, MAX_PACKET_LENGTH - 5);
                                        sending_len = MAX_PACKET_LENGTH - 5 + 2;
                                    }
                                    else 
                                    {
                                        memcpy(sendbuf, &fu_indicator, 1);
                                        memcpy(sendbuf + 1, &fu_type, 1);
                                        memcpy(sendbuf + 2, &sendData[t * MAX_PACKET_LENGTH], MAX_PACKET_LENGTH + 2);
                                        sending_len = MAX_PACKET_LENGTH + 2;
                                    }
                                    status = session.SendPacket((void *)sendbuf, MAX_PACKET_LENGTH + 2);
                                    if (status < 0) {
                                        std::cout << "Error:" << jrtplib::RTPGetErrorString(status) << std::endl;
                                    }
                                }
                                else if((k == t && l > 0 ) || ( t== (k - 1) && l == 0)) 
                                {
                                    fu_type |= 0x40;
                                    session.SetDefaultMark(true);
                                    int i_sendlen = 0;
                                    if (l > 0) 
                                    { 
                                        i_sendlen = sendlen - t * MAX_PACKET_LENGTH;
                                    }
                                    else
                                        i_sendlen = MAX_PACKET_LENGTH; 
    
                                    memcpy(sendbuf, &fu_indicator, 1);
                                    memcpy(sendbuf + 1, &fu_type, 1);
                                    memcpy(sendbuf + 2, &sendData[t * MAX_PACKET_LENGTH], i_sendlen);
printf("raw data:\n\n");
for(uint32_t ii = 0; ii < i_sendlen + 2; ii++) {
    printf(" %x", sendbuf[ii]);
}
printf("\n\n");
                                    status = session.SendPacket((void *)sendbuf ,i_sendlen + 2);
                                    if (status < 0) {
                                        std::cout << "ERROR:" << jrtplib::RTPGetErrorString(status) << std::endl;
                                    }
                                } 
                                t++;
                            }
                        }
                        //jrtplib::RTPTime::Wait(0.0003);
                        av_packet_unref(&h264Packet);
                    }
                    av_packet_unref(&h264Packet);

                }

            } 
        }
    }     
    av_free(packet);           

    return 0;
}
