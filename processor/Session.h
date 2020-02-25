#ifndef __SESSION_H__
#define __SESSION_H__

#include <cstdio>
#include "Common.h"
#include <string>
#include "Connection.h"
#include "Message.h"
#include "MessageBuilder.h"
#include "ikcp.h"

class Session {
private:
    uv_loop_t *__loop;
    uv_udp_t * serv;
    SockAddrIn *__addr;
    std::string __host;
    uint32_t __port;

    Conv __down_conv;
    Timer __timer;

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
    SockAddrIn *__down_addr;   
    Connection *__down_conn;
 
    uint64_t __down_t_interval;
    std::string __key;
    ServType __serv_type;
 
public:
    Session(uv_loop_t *, uv_udp_t *);
    virtual ~Session();

private:
    bool __initConfig();
    bool __initServer();
    bool __initPeerNode();

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
    
public:
    void updateLiveInfo(std::string, std::string);
    int8_t getLiveInfo(std::string&, std::string &);

    void updateUid2Conv(std::string, Conv);
    int8_t getConvByUid(std::string &, Conv &);

    int8_t getConnByConv(Conv &, Connection **conn);

    void releaseConn(Conv &, Connection **conn);

    void deleteUserOrRoom(std::string &, std::string &);

    int8_t cloneMembersById(std::string &, std::set<std::string> &);

private:
    void __procInnerHeartbeat();
};

#endif
