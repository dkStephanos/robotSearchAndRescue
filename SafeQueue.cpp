/**
 * ---------------------------------------------------------------------------
 * File name: SafeQueue.cpp
 * Project name: Project 5
 * Purpose: Provides the methods for the SafeQueue class enabling safe queueing and dequeue by
 *          leveraging mutexes and sempahors
 * ---------------------------------------------------------------------------
 * Creator's name and email: Koi Stephanos, stephanos@etsu.edu
 * Course:  Operating Systems
 * Creation Date: 4/24/2019
 * ---------------------------------------------------------------------------
*/

#include "SafeQueue.h"


SafeQueue::SafeQueue() {
    mutex = PTHREAD_MUTEX_INITIALIZER;
    sem_init(&semaphor, 0, 1);
}

void SafeQueue::enqueue(Message m) {
    sem_wait(&semaphor);
        pthread_mutex_lock(&mutex);
            messagequeue.push(m);
        pthread_mutex_unlock(&mutex);
    sem_post(&semaphor);
}

Message SafeQueue::dequeue() {
    Message msg;

    sem_wait(&semaphor);
        pthread_mutex_lock(&mutex);
            msg = messagequeue.front();
            messagequeue.pop();
        pthread_mutex_unlock(&mutex);
    sem_post(&semaphor);

    return msg;
}

bool SafeQueue::isEmpty() {
    if(messagequeue.empty()) {
        return true;
    } else {
        return false;
    }
}