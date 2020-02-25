#ifndef __HEARTBEAT_HANDLE_H__
#define __HEARTBEAT_HANDLE_H__

#include "ForwardMesgHandle.h"
#include "HandleFactory.h"

class HeartbeatHandle: public ForwardMesgHandle {
public:
    void handleMesg(Session *, Connection *, MesgWrapper *);
    void buildRespMesg(MesgWrapper *, std::string &, void *);
};

REGISTER_HANDLE(HeartbeatHandle, HEARTBEAT_MESSAGE_NO);

#endif
