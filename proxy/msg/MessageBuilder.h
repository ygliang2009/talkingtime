#ifndef __MESSAGE_FACTORY_H__
#define __MESSAGE_FACTORY_H__
#include "Message.h"


class MessageBuilder {
public:
    MessageBuilder(std::string no_, MesgCallback cb_) {
        MessageFactory::getInstance().regCallback(no_, cb_);
    }
};

#ifndef REGISTER_OFF
    #ifndef REGISTER_MESSAGE
        #define REGISTER_MESSAGE(Msg, No) \
           \
           Message* get_##Msg() { \
               return new Msg(); \
           } \
           \
           MessageBuilder mf_##Msg(No,  get_##Msg);
    #endif
#else
    #define REGISTER_MESSAGE(Msg, No) 
#endif

#endif
