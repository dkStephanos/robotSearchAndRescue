/**
 * ---------------------------------------------------------------------------
 * File name: SafeQueue.h
 * Project name: Project 5
 * Purpose: Provides the structure for the SafeQueue class and holds the definition of our Message struct
 * ---------------------------------------------------------------------------
 * Creator's name and email: Koi Stephanos, stephanos@etsu.edu
 * Course:  Operating Systems
 * Creation Date: 4/24/2019
 * ---------------------------------------------------------------------------
*/
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

    bool isEmpty();

};


#endif //HW5_SAFEQUEUE_H
