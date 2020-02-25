#ifndef __MEDIASESSION_H__
#define __MEDIASESSION_H__
#include "rtpsession.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtplibraryversion.h"
#include "rtpsourcedata.h"
#include "RtpContainer.h"
#include "VideoCodec.h"
#include "RtpPacketQue.h"
#include "H264.h"

class MediaSession: public jrtplib::RTPSession {
public:
    void CreateUserContext();

protected:
    void OnPollThreadStep(); 

private:
    void __ProcessRTPPacket(jrtplib::RTPSourceData *, jrtplib::RTPPacket *);

#if 0
private:
    RtpContainer *rc;
    VideoCodec *vc;
#endif

private:
    std::map<uint64_t, RtpPacketQueue *> __ssrc_2_queue;
};
#endif
