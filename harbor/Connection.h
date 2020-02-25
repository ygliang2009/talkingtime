#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "Common.h"
#include "ikcp.h"

enum ConnType {
    CONN_UNKNOWN,
    CONN_UP,
    CONN_DOWN
};

class Connection {
public:
    Connection();
    ~Connection();

public:
    SockAddr *addr;
    ikcpcb *kcp_handle;

private:
    uint64_t __tick;

public:
    ConnType conn_type;

    std::string rid;
    std::string uid;
    uint64_t connect_addr;
    uint64_t connect_port;

public:
    int sendData(const char *, uint32_t);
   
    inline uint64_t tick() {
        return __tick;
    } 
 
    inline void setTick(uint64_t tick_) {
        __tick = tick_;
    }
};
#endif
