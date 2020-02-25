#ifndef __COMMON_H__
#define __COMMON_H__
#include <glog/logging.h>
#include <sys/socket.h>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <fstream>

#ifndef SAFE_FREE
#define SAFE_FREE(p) \
do { \
     if ((p) != NULL) { \
         free(p); \
         (p) = NULL; \
      } \
} while (0)  

#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) \
do { \
     if (p) { \
         delete (p); \
         (p) = NULL; \
      }\
}while (0)

#endif

#define CONNECTION_TIMEOUT	3000*1000

#define MTU	1500

typedef struct sockaddr_in SockAddrIn;
typedef struct sockaddr SockAddr;

typedef uint32_t Conv;
typedef int (*kcp_send_cb)(const char *buf, int len, struct IKCPCB *kcp, void *user);

extern uint64_t MsClock();

extern std::map<std::string, std::string>* ReadConfig(std::string &);
 
enum Direction {
    UNKNOWN, UP, DOWN
};

enum ServType {
    ST_UNDEFINED, ST_PROXY, ST_PROCESSOR, ST_RELAY, ST_HARBOR, ST_RECORDER
};

enum MediaType {
    MT_VIDEO, MT_AUDIO
};

extern ServType i_to_servtype(int32_t serv_type);

#endif
