#ifndef __INNER_HEARTBEAT_HANDLE_H__
#define __INNER_HEARTBEAT_HANDLE_H__

#include "Handle.h"
#include "HandleFactory.h"

class InnerHeartbeatHandle: public Handle {
public:
    void handleMesg(Session *, Connection *, MesgWrapper *);
    void buildRespMesg(MesgWrapper *, std::string &, void *);
};

REGISTER_HANDLE(InnerHeartbeatHandle, INNER_HEARTBEAT_MESSAGE_NO);

#endif
