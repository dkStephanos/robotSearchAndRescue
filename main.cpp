/**
 * ---------------------------------------------------------------------------
 * File name: main.cpp
 * Project name: Project 4
 * Purpose: Creates child 'robot' threads that take commands from the parent and returns their results
 *          to the parent which then forwards logging information to the forked log process
 * ---------------------------------------------------------------------------
 * Creator's name and email: Koi Stephanos, stephanos@etsu.edu
 * Course:  Operating Systems
 * Creation Date: 3/24/2019
 * ---------------------------------------------------------------------------
*/
#include "Log.h"
#include "Board.h"
#include "Robot.h"
#include "SafeQueue.h"
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


const int MAX_ROBOTS = 5;
Message msgs[MAX_ROBOTS];
Message msg = {0, "Blagaga"};
string robotupdate;
Robot robots[MAX_ROBOTS];
SafeQueue parentqueue;
SafeQueue robotqueues[MAX_ROBOTS];
pthread_t robots_ts[MAX_ROBOTS];

void *robotThreadWork(void*) {
    for (int i = 0; i < MAX_ROBOTS; i++) {
        if(robots_ts[i] == pthread_self()) {
            while(true) {
                //Dequeue the instruction
                msgs[i] = robotqueues[i].dequeue();

                //Logs what the robot received from queue
                printf("Robot #%d received: %s\n", (i + 1), msgs[i].payload);

                //If we get the Q command, break the loop
                if(strcmp(msgs[i].payload, "Q") == 0) {
                    break;
                }

                //Take the direction from the end of command and pass to getPosition robot method
                robots[i].updatePosition(msgs[i].payload[strlen(msgs[i].payload) -1]);
                int *position = robots[i].getPosition();
                //Build update string and write to parent pipe
                robotupdate = "P " + std::to_string(i + 1) + " " + std::to_string(position[0]) + " " + std::to_string(position[1]);
                strcpy(msgs[i].payload, robotupdate.c_str());
                msgs[i].payload[strlen(msgs[i].payload)] = '\0';

                //Enqueue the result
                parentqueue.enqueue(msgs[i]);
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
    string setupfilename, cmdfilename, line, servername, portnumber;

    //If log file name is present in command line arguments, set the logfilename for log1
    util_funcs::checkForLogFileAndSetLogFileName(argv, argc, log1);

    //Check for and open setupfile, if return -1 something went wrong so exit.
    if(util_funcs::checkForSetupFile(argv, argc, setupfilename, log1, setupfile) == -1) {
        return 0;
    }

    //Process Setup instructions
    if(util_funcs::processSetupInstructions(log1, setupfile, line, board, servername, portnumber) == -1) {
        cout << "Bad Setup File\n";
        return 0;
    }

    cout << "Server name: " << servername << "  Port number: " << portnumber << "\n";

    const int NUMBER_OF_ROBOTS = board.numrobots;       //sets number of robots const to the value from the board setup
    std::vector<string> cmdline_commands;
    std::vector<string> robotcommands[NUMBER_OF_ROBOTS];
    int numberofcommands = 0;
    int numberofcommandsreceived = 0;
    pid_t logPID;
    pid_t parent = ::getpid();
    struct addrinfo *myinfo; // Address record
    int sockdesc;
    char hostname[81];
    char portnum[81];
    int connection;
    int value;

    //Copy over server info we collected from setupfile
    strcpy(hostname, servername.c_str());
    strcpy(portnum, portnumber.c_str());

    // Use AF_UNIX for unix pathnames instead
    // Use SOCK_DGRAM for UDP datagrams
    cout << "Before socket" << endl;
    sockdesc = socket(AF_INET, SOCK_STREAM, 0);
    if ( sockdesc < 0 )
    {
        cout << "Error creating socket" << endl;
        exit(0);
    }

    // Set up the address record
    cout << "Before getaddrinfo" << endl;
    if ( getaddrinfo(hostname, portnum, NULL, &myinfo) != 0 )
    {
        cout << "Error getting address" << endl;
        exit(0);
    }

    // Connect to the host
    cout << "Before connect" << endl;
    connection = connect(sockdesc, myinfo->ai_addr, myinfo->ai_addrlen);
    if ( connection < 0 )
    {
        cout << "Error in connect" << endl;
        cout << "Sockdesc: " << sockdesc << "  Addrinfo: " << myinfo  << endl;
        exit(0);
    }
    cout << "Client connection = " << connection << endl;

    //Send the name of the log file to the server
    strcpy(msg.payload, log1.getLogfileName().c_str());
    write(sockdesc, (char*)&msg, sizeof(Message));

    //Log the parent ID and number of threads
    printf ("Parent is %d, num threads = %d\n", parent, NUMBER_OF_ROBOTS);

    //Check for cmdfile, getting input from user if not found
    util_funcs::checkForCommandFile(argv, argc, cmdfilename, log1, cmdfile, line, cmdline_commands);

    //Process Command instructions building a vector for each Child/Robot
    util_funcs::processCommandInstructions(log1, cmdfile, cmdline_commands, line, robotcommands, NUMBER_OF_ROBOTS);


    //Parent code
    if(true) {
        //Write all robot commands from robotcommands
        for (int i = 0; i < NUMBER_OF_ROBOTS; i++) {
            for (int j = 0; j < robotcommands[i].size(); j++) {
                numberofcommands++;
                strcpy(msgs[i].payload, robotcommands[i][j].c_str());
                msgs[i].payload[strlen(msgs[i].payload)] = '\0';

                robotqueues[i].enqueue(msgs[i]);

                printf("Parent pushed %s into queue: %d\n", msgs[i].payload, i);
            }

            strcpy(msgs[i].payload, "Q");
            robotqueues[i].enqueue(msgs[i]);
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
        //Loop through parents queue and pass on to logging process
        while(numberofcommandsreceived < numberofcommands) {
            if (!parentqueue.isEmpty()) {
                msg = parentqueue.dequeue();

                printf("Parent received: %s\n", msg.payload);
                msg.payload[strlen(msg.payload)] = '\0';

                // Display the message to be sent
                cout << "Client sends: " << msg.payload << endl;

                // Send the message to the server
                write(sockdesc, (char*)&msg, sizeof(Message));
                numberofcommandsreceived++;
            }
        }
        //We're done logging, so write Q command
        strcpy(msg.payload, "Q");
        // Send the message to the server
        write(sockdesc, (char*)&msg, sizeof(Message));

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
    return 0;
}