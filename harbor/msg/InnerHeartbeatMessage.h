#ifndef __INNER_HEARTBEAT_MESSAGE_H__
#define __INNER_HEARTBEAT_MESSAGE_H__

#include "MessageBuilder.h"

class InnerHeartbeatMessage: public Message {
public:
    InnerHeartbeatMessage():Message() {}
    ~InnerHeartbeatMessage() {}

    void feedMesg(std::string);

public:
    uint64_t last_ts;
    std::string key;
    ServType serv_type;
    std::string host;
    uint32_t port;
};

REGISTER_MESSAGE(InnerHeartbeatMessage, INNER_HEARTBEAT_MESSAGE_NO);
#endif
