#ifndef __UNICAST_MESSAGE_H__
#define __UNICAST_MESSAGE_H__

#include "MessageBuilder.h"
/*
 * msg format: magic_no msg_t rid uid to_uid to_mesg 
 */
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
