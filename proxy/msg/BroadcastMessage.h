#ifndef __BROADCAST_MESSAGE_H__
#define __BROADCAST_MESSAGE_H__

#include "MessageBuilder.h"

class BroadcastMessage: public Message {
public:
    BroadcastMessage():Message() {}
    ~BroadcastMessage() {}

    void feedMesg(std::string);

public:
    std::string to_mesg;
};

REGISTER_MESSAGE(BroadcastMessage, BROADCAST_MESSAGE_NO);
#endif
