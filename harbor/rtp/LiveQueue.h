#ifndef __LIVE_QUEUE_H__
#define __LIVE_QUEUE_H__
#include "RTPWrapper.h"
#include <thread>
#include <queue>
#include <functional>
#include "Common.h"

class Session;

class LiveQueue {
public:
    LiveQueue(MediaType mt);
    ~LiveQueue();

private:
    std::thread *thrd;
    std::mutex __mutex;
    std::queue<RTPWrapper *> __buffer;
    Session *sess;
  
private:
    MediaType __media_t;

public:
    void push(RTPWrapper *);
    void run();
    void bindSession(Session *);
};

#endif
