#ifndef __HANDLE_BUILDER_H__
#define __HANDLE_BUILDER_H__
#include "Handle.h"

class HandleBuilder {
public:
    HandleBuilder(std::string no_, HandleCallback cb_) {
        HandleFactory::getInstance().packHandle(no_, cb_);
    }
};

#ifndef REGISTER_HANDLE 

#define REGISTER_HANDLE(MsgHandle, No) \
\
Handle* reg_##MsgHandle () { \
    return new MsgHandle(); \
} \
HandleBuilder hb_##MsgHandle(No, reg_##MsgHandle);

#endif

#endif
