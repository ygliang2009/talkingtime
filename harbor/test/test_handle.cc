#include "Handle.h"

int main() {
    Handle* hd = HandleFactory::getInstance().getHandle("200");
    hd->handleMesg();
    hd = HandleFactory::getInstance().getHandle("201");
    hd->handleMesg();
    return 0;
}
