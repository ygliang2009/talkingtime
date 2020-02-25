#ifndef __LIVEINFO_HANDLE_H__
#define __LIVEINFO_HANDLE_H__

#include "ForwardMesgHandle.h"
#include "HandleFactory.h"

class LiveInfoHandle: public ForwardMesgHandle {
public:
    void handleMesg(Session*, Connection *, MesgWrapper *);
    void buildRespMesg(MesgWrapper *, std::string &, void *);
};

REGISTER_HANDLE(LiveInfoHandle, LIVEINFO_MESSAGE_NO);

#endif
