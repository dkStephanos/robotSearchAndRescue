//
// Created by Koi Stephanos on 2019-04-25.
//

#ifndef HW5_SAFEQUEUE_H
#define HW5_SAFEQUEUE_H

#include <semaphore.h>
#include <queue>
#include <string>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


struct Message {
    int from;
    char payload[32];
};

class SafeQueue {

private:

    std::queue<Message> messagequeue;
    pthread_mutex_t mutex;
    sem_t semaphor;

public:

    SafeQueue( );

    void enqueue(Message m);

    Message dequeue();

};


#endif //HW5_SAFEQUEUE_H
