#ifndef __HANDLE_H__
#define __HANDLE_H__
#include "Message.h"
#include "Session.h"

class Handle {
public:
    virtual void handleMesg(Session *, Connection *, MesgWrapper *) = 0;
    virtual void buildRespMesg(MesgWrapper *, std::string &, void *) = 0;
};

typedef Handle* (*HandleCallback) ();

class HandleFactory {
private:
    HandleFactory() {};

public:
    static HandleFactory& getInstance() {
        static HandleFactory hf_;
        return hf_;
    }
public:
    void packHandle(std::string no_, HandleCallback cb_) {
        handle_pack_.insert(std::make_pair(no_, cb_));
    } 

    Handle* getHandle(std::string no_) {
        std::map<std::string, HandleCallback>::iterator it = handle_pack_.find(no_);
        if (it == handle_pack_.end()) {
             return NULL;
        }
        return it->second();
    }
private:
    std::map<std::string, HandleCallback> handle_pack_;
};
#endif
