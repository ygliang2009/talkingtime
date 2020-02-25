#ifndef __HEARTBEAT_MESSAGE_H__
#define __HEARTBEAT_MESSAGE_H__

#include "MessageBuilder.h"

class HeartbeatMessage: public Message {
public:
    HeartbeatMessage():Message() {}
    ~HeartbeatMessage() {}

    void feedMesg(std::string);

public:
    uint64_t last_ts;
};

REGISTER_MESSAGE(HeartbeatMessage, HEARTBEAT_MESSAGE_NO);
#endif
