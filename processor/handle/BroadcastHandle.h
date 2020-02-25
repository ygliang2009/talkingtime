#ifndef __UNICAST_HANDLE_H__
#define __UNICAST_HANDLE_H__

#include "Handle.h"
#include "HandleFactory.h"

class BroadcastHandle: public Handle {
public:
    void handleMesg(Session *, Connection *, MesgWrapper *);

    void buildRespMesg(MesgWrapper *, std::string &, void *);

    void buildBroadcastMesg(MesgWrapper *, std::string &, void *);
};

REGISTER_HANDLE(BroadcastHandle, BROADCAST_MESSAGE_NO);

#endif
