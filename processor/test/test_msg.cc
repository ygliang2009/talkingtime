#include "Message.h"

int main () {
    //LiveInfoMessage liveMsg;
    Message *msg = MessageFactory::getInstance().getMessage(200);
    msg->feedMessage("aabbcc");
    msg = MessageFactory::getInstance().getMessage(201);
    msg->feedMessage("ddbbcc");
    return 0;
}
