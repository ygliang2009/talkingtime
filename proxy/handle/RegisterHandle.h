#ifndef __REGISTER_HANDLE_H__
#define __REGISTER_HANDLE_H__

#include "ForwardMesgHandle.h"
#include "HandleFactory.h"

class RegisterHandle: public ForwardMesgHandle {
public:
    void handleMesg(Session *, Connection *, MesgWrapper *);
    void buildRespMesg(MesgWrapper *, std::string &, void *);
};

REGISTER_HANDLE(RegisterHandle, REGISTER_MESSAGE_NO);

#endif
