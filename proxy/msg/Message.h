#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "MessageType.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "Common.h"

class Message {
public:
    Message();
    virtual ~Message();

public:
    virtual void feedMesg(std::string);
    bool inline isValid() const {return is_valid;}
    
    void setServType(ServType st){
        serv_type = st;
    }

    ServType getServType() const {
        return serv_type;
    }

public:
    std::string raw_msg;
    std::vector<std::string> msg_bar;
    std::string rid;
    std::string uid;
    std::string msg_t;

    bool is_valid;
    static const std::string Magic_NO;
    static const std::string ToMagic_NO;
    static const char Splitter;

public:
    Direction direct;
    ServType serv_type;
};

typedef Message* (*MesgCallback) ();


class MesgWrapper {
public:
    MesgWrapper(Conv conv_, Message *msg_) {
        conv = conv_;
        msg = msg_;
    }
    virtual ~MesgWrapper();


public:
    Conv conv;
    Message *msg;
};


class MessageFactory {
private:
    MessageFactory() {};

public:
    std::map<std::string, MesgCallback> cb_map;

public:
    void regCallback(std::string no_, MesgCallback cb_) {
        cb_map.insert(std::make_pair(no_, cb_));
    }

    static MessageFactory& getInstance() {
        static MessageFactory fact;
        return fact;
    }

    Message* getMessage(std::string no_) {
        std::map<std::string, MesgCallback>::iterator it = cb_map.find(no_);
        if (it == cb_map.end())
            return NULL;
        return (it->second)();
    } 
};


#endif
