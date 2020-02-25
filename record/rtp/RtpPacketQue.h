#ifndef __FRAME_QUEUE_H__
#define __FRAME_QUEUE_H__
#include "RTPWrapper.h"
#include <thread>
#include <queue>
#include <functional>
#include "Common.h"
#include "RtpContainer.h"
#include "VideoCodec.h"
#include "H264.h"

class RtpPacketQueue {
public:
    RtpPacketQueue(MediaType mt);
    ~RtpPacketQueue();

private:
    std::thread *thrd;
    std::mutex __mutex;
    std::queue<RTPWrapper *> __buffer;
    VideoCodec *__video_codec;
    RtpContainer *__container;
  
private:
    MediaType __media_t;

public:
    void push(RTPWrapper *);
    void run();
    
    void buildVideoPacket(RTPWrapper *);
};

#endif
