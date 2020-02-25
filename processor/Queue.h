#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <queue>
#include <pthread.h>
#include <stdexcept>

template <class T> class TinyMesgQueue {
private:
    std::queue<T> queue_;
    pthread_mutex_t qlock_;

public:
    TinyMesgQueue<T>();
    virtual ~TinyMesgQueue<T>();

public:
    void enQueue(T);
    T deQueue();
    uint32_t size();
    bool empty();
};

template <class T> TinyMesgQueue<T>::TinyMesgQueue() {

}

template <class T> TinyMesgQueue<T>::~TinyMesgQueue() {

}

template <class T> void TinyMesgQueue<T>::enQueue(T elem) {
    pthread_mutex_lock(&qlock_);
    queue_.push(elem);
    pthread_mutex_unlock(&qlock_);
}

template <class T> T TinyMesgQueue<T>::deQueue() {
    if (queue_.size() <= 0) {
        throw std::out_of_range("Queue<>::deQueue: empty queue");
    } 
    pthread_mutex_lock(&qlock_);

    T t = queue_.front();
    queue_.pop();

    pthread_mutex_unlock(&qlock_);
    return t;
}

template <class T> uint32_t TinyMesgQueue<T>::size() {
    return queue_.size();
}

template <class T> bool TinyMesgQueue<T>::empty() {
    return queue_.empty();
}
#endif
