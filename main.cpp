/**
 * ---------------------------------------------------------------------------
 * File name: main.cpp
 * Project name: Project 4
 * Purpose: Creates child 'robot' threads that take commands from the parent and returns their results
 *          to the parent which then forwards logging information to the forked log process
 * ---------------------------------------------------------------------------
 * Creator's name and email: Koi Stephanos, stephanos@etsu.edu
 * Course:  Operating Systems
 * Creation Date: 2/12/2019
 * ---------------------------------------------------------------------------
*/
#include "Log.h"
#include "Board.h"
#include "Robot.h"
#include "utilities.cpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <cstring>
#include <queue>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

struct Message {
    int from;
    char payload[32];
};

const int MAX_ROBOTS = 5;
Message msg{0, "Blagaga"};
string robotupdate;
Robot robots[MAX_ROBOTS];
queue<Message> parentqueue;
queue<Message> robotqueues[MAX_ROBOTS];
pthread_mutex_t parentlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t robotlocks[MAX_ROBOTS];      //Leave enough space for the maximum robot amount, we will initialize later
sem_t parentsem;
sem_t robotsems[MAX_ROBOTS];
pthread_t robots_ts[MAX_ROBOTS];

void *robotThreadWork(void*) {
    printf("I am robot thread id = %d\n", pthread_self());
    for (int i = 0; i < 5; i++) {
        if(robots_ts[i] == pthread_self()) {
            while(true) {
                //Dequeue the instruction
                sem_wait(&robotsems[i]);
                    pthread_mutex_lock(&robotlocks[i]);
                        msg = robotqueues[i].front();
                        robotqueues[i].pop();
                    pthread_mutex_unlock(&robotlocks[i]);
                sem_post(&robotsems[i]);

                //Logs what the robot received from queue
                printf("Robot #%d received: %s\n", (i + 1), msg.payload);

                //If we get the Q command, break the loop
                if(strcmp(msg.payload, "Q") == 0) {
                    break;
                }

                //Take the direction from the end of command and pass to getPosition robot method
                robots[i].updatePosition(msg.payload[strlen(msg.payload) -1]);
                int *position = robots[i].getPosition();
                //Build update string and write to parent pipe
                robotupdate = "P " + std::to_string(i + 1) + " " + std::to_string(position[0]) + " " + std::to_string(position[1]);
                strcpy(msg.payload, robotupdate.c_str());
                msg.payload[strlen(msg.payload)] = '\0';

                //Enqueue the result
                sem_wait(&parentsem);
                    pthread_mutex_lock(&parentlock);
                        parentqueue.push(msg);
                    pthread_mutex_unlock(&parentlock);
                sem_post(&parentsem);
            }
        }
    }
    //Unlocks mutex, dequeue instruction, if Q, exit, otherwise make move, and enqueue current position, re-lock mutex
    pthread_exit(0);
}

int main(int argc, char** argv) {

    Board board;
    Log log1;
    std::ifstream setupfile, cmdfile;
    string setupfilename, cmdfilename, line;

    //If log file name is present in command line arguments, set the logfilename for log1
    util_funcs::checkForLogFileAndSetLogFileName(argv, argc, log1);

    //Check for and open setupfile, if return -1 something went wrong so exit.
    if(util_funcs::checkForSetupFile(argv, argc, setupfilename, log1, setupfile) == -1) {
        return 0;
    }

    //Process Setup instructions
    if(util_funcs::processSetupInstructions(log1, setupfile, line, board) == -1) {
      cout << "Bad Setup File\n";
      return 0;
    }

    const int NUMBER_OF_ROBOTS = board.numrobots;
    std::vector<string> cmdline_commands;
    std::vector<string> robotcommands[NUMBER_OF_ROBOTS];
    int logpipe[2];
    int pipes_count = 0;
    pid_t logPID;
    pid_t parent = ::getpid();
    sem_init(&parentsem, 0, 0);
    //Loop through robot locks/semaphors and initialize them
    for (int i = 0; i < NUMBER_OF_ROBOTS; i++) {
        robotlocks[i] = PTHREAD_MUTEX_INITIALIZER;
        sem_init(&robotsems[i], 0, 0);
    }

    //Log the parent ID and number of threads
    printf ("Parent is %d, num threads = %d\n", parent, NUMBER_OF_ROBOTS);

    //Check for cmdfile, getting input from user if not found
    util_funcs::checkForCommandFile(argv, argc, cmdfilename, log1, cmdfile, line, cmdline_commands);

    //Process Command instructions building a vector for each Child/Robot
    util_funcs::processCommandInstructions(log1, cmdfile, cmdline_commands, line, robotcommands, NUMBER_OF_ROBOTS);

    //Create log pipe for the parent to send updates through so they can be logged
    pipe(logpipe);

    //Parent code
    if(true) {
        msg.from = 0;
      //Write all robot commands from robotcommands
      for (int i = 0; i < NUMBER_OF_ROBOTS; i++) {
          for (int j = 0; j < robotcommands[i].size(); j++) {
              strcpy(msg.payload, robotcommands[i][j].c_str());
              msg.payload[strlen(msg.payload)] = '\0';

              sem_wait(&robotsems[i]);
                pthread_mutex_lock(&robotlocks[i]);
                    robotqueues[i].push(msg);
                pthread_mutex_unlock(&robotlocks[i]);
              sem_post(&robotsems[i]);

              printf("Parent pushed %s into queue: %d\n", msg.payload, i);
          }
          strcpy(msg.payload, "Q");
          sem_wait(&robotsems[i]);
            pthread_mutex_lock(&robotlocks[i]);
                robotqueues[i].push(msg);
            pthread_mutex_unlock(&robotlocks[i]);
          sem_post(&robotsems[i]);
      }
      //Create the robot processes, storing their pid's for parent to wait on
      for (int i = 0 ; i < NUMBER_OF_ROBOTS ; i++)  {
          if( pthread_create(&robots_ts[i], NULL, robotThreadWork, NULL) ) {
              printf("Thread creation failed.\n");
              exit(0);
          }
      }
    }

    //Parent Code
    if(parent == ::getpid()) {
      //Create the process for the log
      logPID = fork();
      //Log code
      if(logPID == 0) {
        //Close unused write end of logpipe and open logfile
        close(logpipe[1]);
        log1.open();
        //Read all the updates from the parent and print them to the log
        while(read(logpipe[0], (void*)&msg, sizeof(Message)) > 0) {
            printf("Log received: %s\n", msg.payload);
            //util_funcs::sendToLog(log1, string(msg.payload));
            log1.writeLogRecord(string(msg.payload));
        }
        //Close read end of logpipe and close logfile
        close(logpipe[0]);
        log1.close();
      } else {  //More parent code
        close(logpipe[0]);
        //Loop through parents queue and pass on to logging process
        while(!parentqueue.empty()) {
            sem_wait(&parentsem);
                pthread_mutex_lock(&parentlock);
                    msg = parentqueue.front();
                    parentqueue.pop();
                pthread_mutex_unlock(&parentlock);
            sem_post(&parentsem);

            printf("Parent received: %s\n", msg.payload);
            msg.payload[strlen(msg.payload)] = '\0';
            write(logpipe[1], (void*)&msg, sizeof(msg));
        }
        //We're done logging, so close write end of logpipe
        close(logpipe[1]);
        //Wait on threads to join...
        for (int i = 0; i < NUMBER_OF_ROBOTS; i++) {
          cout << "Waiting for thread: to finish" << endl;
          pthread_join(robots_ts[i], NULL);               // Wait for p to finish
          cout << "Thread: has shut down" << endl;
        }
        cout << "Waiting for Log: " << logPID << " to finish" << endl;
        waitpid(logPID, NULL, 0);
        cout << "Log: " << logPID << " has shut down" << endl;
      }
    }
    return 0;
}