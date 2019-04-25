//
// Created by Koi Stephanos on 2019-04-25.
//

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