#ifndef __UNICAST_MESSAGE_H__
#define __UNICAST_MESSAGE_H__

#include "MessageBuilder.h"

class UnicastMessage: public Message {
public:
    UnicastMessage():Message() {}
    ~UnicastMessage() {}

    void feedMesg(std::string);

public:
    std::string to_uid;
    std::string to_mesg;
};

REGISTER_MESSAGE(UnicastMessage, UNICAST_MESSAGE_NO);
#endif
