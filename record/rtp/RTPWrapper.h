#ifndef __RTP_WRAPPER_H__
#define __RTP_WRAPPER_H__
#include "rtpsession.h"
#include "rtpsourcedata.h"
#include "Common.h"

class RTPWrapper{
public:
    RTPWrapper(){ 
        rtp_pack = NULL;
    }
    virtual ~RTPWrapper(){
        SAFE_DELETE(rtp_pack);
    }

public:
    jrtplib::RTPPacket *rtp_pack;
    jrtplib::RTPSourceData *src_data;
};
#endif
