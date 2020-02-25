#ifndef __USER_JOIN_MESSAGE_H__
#define __USER_JOIN_MESSAGE_H__

#include "MessageBuilder.h"

class UserJoinMessage: public Message {
public:
    UserJoinMessage():Message() {}
    ~UserJoinMessage() {}

    void feedMesg(std::string);

public:
    uint32_t ssrc;
    uint32_t video_defined_port;
    uint32_t audio_defined_port;
};

REGISTER_MESSAGE(UserJoinMessage, USER_JOIN_MESSAGE_NO);
#endif
