#ifndef __VIDEO_CODEC__
#define __VIDEO_CODEC__

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
#include "libavcodec/avcodec.h"
}
#include "Common.h"

#define AV_CODEC_FLAG_GLOBAL_HEADER (1 << 22)
#define CODEC_FLAG_GLOBAL_HEADER AV_CODEC_FLAG_GLOBAL_HEADER

class VideoCodec {
public:
    VideoCodec();
    virtual ~VideoCodec();

public:
    std::string out_filename;
    bool inited;   

public:
    bool initContext();
    bool buildFrame(unsigned char *, uint64_t len);

private:
    bool __initCodec();    
    bool __initOutCodec();

private:
    AVPacket *__pkt;
    AVFrame *__frame;
    AVCodec *__codec;
    AVCodecContext *__codec_ctx;
    AVFormatContext *__format_ctx;
    AVCodecContext *__out_codec_ctx;
    AVDictionary *__dict_opt;
};
#endif
