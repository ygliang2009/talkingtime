#ifndef __LIVEINFO_MESSAGE_H__
#define __LIVEINFO_MESSAGE_H__

#include "MessageBuilder.h"

class LiveInfoMessage: public Message {
public:
    LiveInfoMessage():Message() {}
    ~LiveInfoMessage() {}
public:
    void feedMesg(std::string);
};

REGISTER_MESSAGE(LiveInfoMessage, LIVEINFO_MESSAGE_NO);

#endif
