#ifndef __FORWARD_MESG_HANDLE_H__
#define __FORWARD_MESG_HANDLE_H__
#include "Handle.h"

class ForwardMesgHandle: public Handle {
public:
    void handleMesg(Session *, Connection *, MesgWrapper *);
    void buildRespMesg(MesgWrapper *, std::string &, void *);
};


#endif 
