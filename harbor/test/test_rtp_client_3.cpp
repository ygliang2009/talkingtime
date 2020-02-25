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

int main (int argc, char *argv[]) {
    if (argc == 1) {
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

    if (status < 0) {
        std::cout << "ERROR:" << jrtplib::RTPGetErrorString(status) << std::endl;
    }

    std::string serverip_str = "127.0.0.1";
    int32_t dest_port = 9866;
    uint32_t dest_ip = inet_addr(serverip_str.c_str());
    dest_ip = ntohl(dest_ip);

    jrtplib::RTPIPv4Address addr(dest_ip,dest_port);
    status = session.AddDestination(addr);

    if (status < 0) {
        std::cout << "ERROR:" << jrtplib::RTPGetErrorString(status) << std::endl;
    }
    session.SetDefaultPayloadType(96);//设置传输类型
    session.SetDefaultMark(true);         //设置位
    session.SetTimestampUnit(1.0/90000.0); //设置采样间隔
    session.SetDefaultTimestampIncrement(3600);//设置时间戳增加间隔 

    av_register_all();
    avdevice_register_all();
    avformat_network_init();

    AVFormatContext *formatContext = nullptr;
    formatContext = avformat_alloc_context();

    AVDictionary *videoOptions;
    int nRet = avformat_open_input(&formatContext, filename, 0, &videoOptions);
    if (nRet < 0) {
        printf("Open file error: %s\n", filename);
        return -1;
    }

    if (avformat_find_stream_info(formatContext, &videoOptions) < 0) {
        printf("Can't find video stream\n");
        return -1;
    }

    int duration = formatContext->duration / AV_TIME_BASE;
    printf("file duration = %d\n", duration);
    uint8_t nStream = formatContext->nb_streams;

    int streamId = -1;
    AVCodecContext *codecContext = nullptr;

    for (int i = 0; i < nStream; i++) {
        codecContext = formatContext->streams[i]->codec;

        if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
            streamId = i;
            AVCodec *vCodec = avcodec_find_decoder(codecContext->codec_id);
            if (vCodec == nullptr) {
                perror("can't find codec");
                continue;
            }
            if(avcodec_open2(codecContext, vCodec, 0) < 0) {
                perror("avcodec open error");
                continue;
            }
            break;
        }
    }
    printf("%s, get width = %d, get height = %d, fmt = %d\n", filename, codecContext->width, codecContext->height, codecContext->pix_fmt);
    
    AVDictionary *outVideoOptions = nullptr;
    AVCodecContext *outCodecCtx = nullptr;

    AVCodec *outAVCodec = avcodec_find_encoder(AV_CODEC_ID_H264);

    if (!outAVCodec) {
        std::cout << "Have not found h264 codec" << std::endl; 
        return 1;
    }
    outCodecCtx = avcodec_alloc_context3(outAVCodec);
    if (!outCodecCtx) {
        return 1;
    }
    outCodecCtx->codec_id = AV_CODEC_ID_H264;
    outCodecCtx ->codec_type = AVMEDIA_TYPE_VIDEO;
    outCodecCtx->pix_fmt  = AV_PIX_FMT_YUV420P;
    outCodecCtx->bit_rate = 400000; // 400000
    outCodecCtx->rc_max_rate = 400000;
    outCodecCtx->rc_min_rate = 400000;
    outCodecCtx->width = codecContext->width;
    outCodecCtx->height = codecContext->height;
    outCodecCtx->gop_size = 100;//3
    AVRational rat = {1,15};
    outCodecCtx->time_base = rat;

    //outCodecCtx->qmax = 51;
    //outCodecCtx->qmin = 10;
    outCodecCtx->max_b_frames = 0;


    if (codecContext->codec_id == AV_CODEC_ID_H264)
    {
        av_dict_set(&outVideoOptions,"tune", "zerolatency", 0);
        av_dict_set(&outVideoOptions,"preset", "slow", 0);
	//av_opt_set(c->priv_data, "superfast", "zerolatency", 0);   //对编码速度有影响
	//av_opt_set(c->priv_data, "preset", "slow", 0);          //对编码速度有影响
	//av_dict_set(&outVideoOptions, "preset", "ultrafast", 0);     //对编码速度有影响
	//av_opt_set(c->priv_data, "tune","stillimage,fastdecode,zerolatency",0);
	//av_opt_set(c->priv_data, "x264opts","crf=26:vbv-maxrate=728:vbv-bufsize=364:keyint=25",0);
    }
    int value = 0;
    value = avcodec_open2(outCodecCtx, outAVCodec, &outVideoOptions);
    if(value < 0) {
        std::cout << "avcodec open2 error" << std::endl;
        return 1;
    }

    if (value < 0) {
        return 1;
    } 

    AVFrame *h264Frame = av_frame_alloc();
    h264Frame->format = outCodecCtx->pix_fmt;
    h264Frame->width = outCodecCtx->width;
    h264Frame->height = outCodecCtx->height;
    h264Frame->pts = 0;

    int h264_outbuf_size;
    int nbytes = avpicture_get_size(outCodecCtx->pix_fmt, outCodecCtx->width, outCodecCtx->height);
    uint8_t *h264_outbuf = (uint8_t*)av_malloc(nbytes);
    avpicture_fill((AVPicture *)h264Frame, h264_outbuf, AV_PIX_FMT_YUV420P, outCodecCtx->width, outCodecCtx->height);

    SwsContext* swsCtx_= sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt, outCodecCtx->width, outCodecCtx->height, outCodecCtx->pix_fmt, SWS_BICUBIC, nullptr, nullptr, nullptr);

    AVPacket h264Packet;

    AVFrame *vframe = av_frame_alloc();
    AVPacket *packet = (AVPacket *) malloc(sizeof(AVPacket));
    int y_size = codecContext->width * codecContext->height;
    //packet 缓存每一帧
    av_new_packet(packet, y_size);

    int ret, got_picture, got_h264_pic;
    int vframe_index = 0;

    while (1) {
        if(av_read_frame(formatContext, packet) >= 0) {
            if (packet->stream_index == streamId) {
                AVStream *in_stream = formatContext->streams[streamId];
                ret = avcodec_decode_video2(codecContext, vframe, &got_picture, packet);
                if (ret < 0) {
                    return -1;
                }
                if (got_picture) {
                    sws_scale(swsCtx_, vframe->data, vframe->linesize, 0, codecContext->height, h264Frame->data, h264Frame->linesize);
                    av_init_packet(&h264Packet);
                    h264Packet.data = nullptr;
                    h264Packet.size = 0;
                    h264Packet.pts = AV_NOPTS_VALUE;
		    h264Packet.dts = AV_NOPTS_VALUE;  
                    //h264Packet.pts = packet->pts;
		    //h264Packet.dts = packet->dts;  
       
                    //h264Frame->pts = vframe->pkt_pts;

                    avcodec_encode_video2(outCodecCtx, &h264Packet, h264Frame, &got_h264_pic);
                    //avcodec_encode_video2(outCodecCtx, &h264Packet, vframe, &got_h264_pic);
                   
                    if(got_h264_pic) {
                        //h264Packet.data[h264Packet.size] = 0x01;
                        if(h264Packet.pts != AV_NOPTS_VALUE) {
                            h264Packet.pts = av_rescale_q(h264Packet.pts, in_stream->codec->time_base, in_stream->time_base);
                        }
                        if(h264Packet.dts != AV_NOPTS_VALUE) {
                            h264Packet.dts = av_rescale_q(h264Packet.dts, in_stream->codec->time_base, in_stream->time_base);
                        }
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

                        if (sendlen < MAX_PACKET_LENGTH) {
                            char sendbuf[1430] = {0};
                            
                            memcpy(sendbuf, sendData, sendlen);
                            status = session.SendPacket(sendbuf, sendlen);
                            if (status < 0) {
                                std::cout << "ERROR:" << jrtplib::RTPGetErrorString(status) << std::endl;
                            }
                        } 
                        else {
                            session.SetDefaultMark(false);
                            int k = 0, l = 0, t = 0;
                            k = sendlen / MAX_PACKET_LENGTH;
                            l = sendlen % MAX_PACKET_LENGTH;

                            while ((t < k) || ((t == k) && (l > 0))) {
                                char sendbuf[1430] = {0};

                                if ((t == 0) || ((t < k) && (t != 0))) {
                                    session.SetDefaultMark(false);
                                    memcpy(sendbuf, &sendData[t * MAX_PACKET_LENGTH], MAX_PACKET_LENGTH);
                                    status = session.SendPacket((void *)sendbuf, MAX_PACKET_LENGTH);
                                    if (status < 0) {
                                        std::cout << "Error:" << jrtplib::RTPGetErrorString(status) << std::endl;
                                    }
                                }
                                else if((k == t && l > 0 ) || ( t== (k - 1) && l == 0)) {
                                    session.SetDefaultMark(true);
                                    int iSendLen;
                                    if (l > 0) { 
                                        iSendLen = sendlen - t * MAX_PACKET_LENGTH;
                                    }
                                    else
                                        iSendLen = MAX_PACKET_LENGTH; 

                                    memcpy(sendbuf, &sendData[t*MAX_PACKET_LENGTH], iSendLen);
                                    status = session.SendPacket((void *)sendbuf ,iSendLen);
                                    if (status < 0) {
                                        std::cout << "ERROR:" << jrtplib::RTPGetErrorString(status) << std::endl;
                                    }
                                } 
                                t++;
                            }
                        }
                        jrtplib::RTPTime::Wait(0.005);
                        av_packet_unref(&h264Packet);
                    }
                    av_packet_unref(&h264Packet);
                }
            } 
        }
        av_free_packet(packet);           
    }     

    return 0;
}
