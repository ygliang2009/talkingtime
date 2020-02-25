#ifndef __SESSION_H__
#define __SESSION_H__

#include <cstdio>
#include "Common.h"
#include <string>
#include "Message.h"
#include "MessageBuilder.h"
#include "User.h"
#include "LiveQueue.h"
#include "RTPWrapper.h"
#include <thread>

/*
    Make sure the position of Connection.h is at last forever, 
    or the containing header ikcp.h will cause the 
    compile error. WTF
*/
#include "Connection.h"

class Session {
private:
    uv_loop_t *__loop;
    uv_udp_t * serv;
    SockAddrIn *__addr;
    std::string __host;
    uint32_t __port;

    Conv __down_conv;
    Timer __timer;

    LiveQueue *__live_vq;
    LiveQueue *__live_aq;

    int __serv_video_sock;
    int __serv_audio_sock;

private:
    uv_alloc_cb alloc_cb;
    uv_udp_recv_cb recv_cb;
    kcp_send_cb on_send_cb;
    uv_udp_send_cb send_cb;
    uv_timer_cb __timer_cb;

private:
    std::map<Conv, Connection*> __conv_2_conn;
    TinyMesgQueue<MesgWrapper *> __msg_que;

    std::map<std::string, std::set<std::string> * > __room_info;
    std::map<std::string, Conv> __uid_2_conv;
    std::map<std::string, std::string> *__config_map;
    std::map<uint32_t, std::string> __ssrc_2_uid;
    std::map<std::string, uint32_t> __uid_2_ssrc;

    std::mutex __ssrc_mutex;
    std::mutex __user_mutex; 

    SockAddrIn *__down_addr;   
    Connection *__down_conn;

    std::map<std::string, User *> __user_info;
 
    uint64_t __down_t_interval;
    std::string __key;
    ServType __serv_type;
    
    uint32_t __src_video_port_base;
    uint32_t __dest_video_port_base;
    uint32_t __src_audio_port_base;
    uint32_t __dest_audio_port_base;
 
public:
    Session(uv_loop_t *, uv_udp_t *);
    virtual ~Session();

private:
    bool __initConfig();
    bool __initServer();
    bool __initPeerNode();

    bool __insertUser(std::string, std::string, uint32_t, uint32_t, uint32_t);
    bool __updateUser(std::string, std::string, uint32_t, uint32_t, uint32_t);

    bool __initMediaServer();
public:
    bool initSess();
    bool startSess();
    
    void setCallback(uv_alloc_cb, uv_udp_recv_cb, kcp_send_cb, uv_udp_send_cb);
    
    inline void setTimerCallback(uv_timer_cb cb) {
        __timer_cb = cb;
    }
 
    inline void setConfigMap(std::map<std::string, std::string> *config_map_) {
        __config_map = config_map_;
    }

    void allocBuff(uv_handle_t *handle, size_t size, uv_buf_t *buff);
    void onRecvPacket(ssize_t, const uv_buf_t *, const SockAddr *, unsigned);
    void onSendPacket(const char *, int, Conv, ikcpcb*);
    
    void onTimer(Timer *);

    void procWrapper(RTPWrapper *, MediaType);
    
public:
    void updateLiveInfo(std::string, std::string);
    int8_t getLiveInfo(std::string&, std::string &);

    bool updateUserInfo(std::string, std::string, uint32_t, uint32_t, uint32_t);   
    bool deleteUserInfo(const std::string &, const std::string &);   

    void updateUid2Conv(std::string, Conv);
    int8_t getConvByUid(const std::string &, Conv &) const;

    int8_t getConnByConv(const Conv &, Connection **conn) const;

    void releaseConn(Conv &, Connection **conn);

    void deleteUserOrRoom(const std::string &, const std::string &);

    int8_t cloneMembersById(std::string &, std::set<std::string> &);

private:
    void __procInnerHeartbeat();
};

#endif
