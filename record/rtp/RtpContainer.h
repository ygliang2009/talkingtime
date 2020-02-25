#ifndef __RTPCONTAINER_H__
#define __RTPCONTAINER_H__
#include <cstdint>
#include "Common.h"
#define		MAX_BUFF_SIZE		1024 * 1024 * 4

class RtpContainer {
public:
    RtpContainer();
    ~RtpContainer();

public:
    bool feedPacket(unsigned char *, uint64_t);
    bool appendPacket(unsigned char *, uint64_t);

    inline void resetPacket() {
        //p_buff = {0};
        memset(p_buff, 0, sizeof(unsigned char) * MAX_BUFF_SIZE);
        plen = 0;
    }

public:
    unsigned char *p_buff;
    uint64_t plen;
};
#endif
