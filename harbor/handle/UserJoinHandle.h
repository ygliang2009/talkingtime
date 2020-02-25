#ifndef __USER_JOIN_HANDLE_H__
#define __USER_JOIN_HANDLE_H__

#include "Handle.h"
#include "HandleFactory.h"

class UserJoinHandle: public Handle {
public:
    void handleMesg(Session *, Connection *, MesgWrapper *);
    void buildRespMesg(MesgWrapper *, std::string &, void *);
};

REGISTER_HANDLE(UserJoinHandle, USER_JOIN_MESSAGE_NO);

#endif
