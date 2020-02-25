#ifndef __RECORD_SESSION_H__
#define __RECORD_SESSION_H__
#include "Common.h"
#include <string>
#include <map>
#include "MediaSession.h"

class RecordSession {
public:
    RecordSession() {

    }
    virtual ~RecordSession();

public:
    inline void setConfigMap(std::map<std::string, std::string> *config_map_) {
        __config_map = config_map_;
    }

    bool createSession();
    
private:
    std::map<std::string, std::string> *__config_map;
    MediaSession __media_sess;

private:
    uint32_t __bind_port;
};
#endif
