#include "Queue.h"
#include <iostream>


int main() {
    TinyMesgQueue<uint32_t> que;
    que.enQueue(111);
    que.enQueue(4444);
    que.enQueue(222);
    std::cout << "dequeue:" << que.deQueue() << std::endl;
    std::cout << "dequeue:" << que.deQueue() << std::endl;
    std::cout << "dequeue:" << que.deQueue() << std::endl;
    return 0;
}
