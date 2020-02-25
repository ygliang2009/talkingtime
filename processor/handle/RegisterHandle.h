#ifndef __REGISTER_HANDLE_H__
#define __REGISTER_HANDLE_H__

#include "Handle.h"
#include "HandleFactory.h"

class RegisterHandle: public Handle {
public:
    void handleMesg(Session *, Connection *, MesgWrapper *);
    void buildRespMesg(MesgWrapper *, std::string &, void *);
};

REGISTER_HANDLE(RegisterHandle, REGISTER_MESSAGE_NO);

#endif
