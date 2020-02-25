#ifndef __USER_H__
#define __USER_H__
#include <string>
#include <list>
#include "MediaSession.h"
#include "LiveQueue.h"
#include "RTPWrapper.h"

class User {
public:
    User();
    virtual ~User();

public:
    std::string uid;
    MediaSession *mSess;

    uint64_t dest_ip;
    uint32_t video_dest_port;   
    uint32_t audio_dest_port;   

public:
    int createMediaSess(uint32_t, uint32_t);

    void setMembers(std::list<User *>);
    void addMember(User *);

    void updateMember(User *);
    void deleteMember(User *);

    void procWrapper(RTPWrapper *, MediaType); 

    void setServVideoSock(int sock) {
        mSess->setServVideoSock(sock);
    }

    void setServAudioSock(int sock) {
        mSess->setServAudioSock(sock);
    }

    void setLiveVideoQue(LiveQueue *que) {
        mSess->setLiveVideoQue(que);
    }

    void setLiveAudioQue(LiveQueue *que) {
        mSess->setLiveAudioQue(que);
    }

};

#endif
