#ifndef __MEDIA_SESSION_H__
#define __MEDIA_SESSION_H__
#include "rtpsession.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtplibraryversion.h"
#include "rtpsourcedata.h"
#include <list>
#include "LiveQueue.h"

class User;

class RTPSession: public jrtplib::RTPSession {
protected:
    #if 0
    virtual void OnValidatedRTPPacket(
        jrtplib::RTPSourceData *srcdat, jrtplib::RTPPacket *rtppack, 
            bool isonprobation, bool *ispackethandled) override {
         *ispackethandled = true;
         RTPWrapper *wrapper = new RTPWrapper();
         wrapper->rtp_pack = rtppack;
         que->push(wrapper);
    }
    #endif
    void OnPollThreadStep() override; 

    void __processRTPPacket(jrtplib::RTPSourceData *, jrtplib::RTPPacket *);

public:
    LiveQueue *que;
    
    void inline setUser(User *usr) {
        user = usr;
    }
    
    void setMembers(std::list<User *>);
    void addMember(User *);

    void updateMember(User *);
    void deleteMember(User *);

    void inline setLiveQue(LiveQueue *que_) {
        que = que_;
    }

private:
    void __resetDestinations();

public:
    std::list<User *> members;
    User *user;
};


class MediaSession {
public:
    MediaSession(){}
    virtual ~MediaSession() {}

    int createSession(uint32_t, uint32_t);

public:
    RTPSession video_sess;
    RTPSession audio_sess;

    int video_sock;
    int audio_sock;

    int serv_video_sock;
    int serv_audio_sock;

public:
    void inline setUser(User *usr) {
        video_sess.setUser(usr);
        audio_sess.setUser(usr);
    }
    
    void inline setMembers(std::list<User *> users) {
        video_sess.setMembers(users);
        audio_sess.setMembers(users);
    }

    void inline addMember(User *user) {
        video_sess.addMember(user);
        audio_sess.addMember(user);
    }

    void updateMember(User *);
    void deleteMember(User *);

    void inline setLiveVideoQue(LiveQueue *que) {
        video_sess.setLiveQue(que);
    }

    void inline setLiveAudioQue(LiveQueue *que) {
        audio_sess.setLiveQue(que);
    }

    void setServVideoSock(int sock) {
        serv_video_sock = sock;
    }

    void setServAudioSock(int sock) {
        serv_audio_sock = sock;
    }


    int forwardPacket(RTPWrapper *, MediaType);

private:
    int __fetchOneSock(MediaType);
};
#endif
