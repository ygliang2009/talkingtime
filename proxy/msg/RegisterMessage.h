#ifndef __REGISTER_MESSAGE_H__
#define __REGISTER_MESSAGE_H__

#include "MessageBuilder.h"

class RegisterMessage: public Message {
public:
    RegisterMessage():Message() {}
    ~RegisterMessage() {}

    void feedMesg(std::string);
};

REGISTER_MESSAGE(RegisterMessage, REGISTER_MESSAGE_NO);
#endif
