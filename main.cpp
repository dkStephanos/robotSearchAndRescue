/**
 * ---------------------------------------------------------------------------
 * File name: main.cpp
 * Project name: Project 3
 * Purpose: Clones the parent process before having the child output the PIDs and the parent launches hw1.
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
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct Message {
    int from;
    char payload[32];
};

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
    Message msg{0, "Blagaga"};
    Robot robots[NUMBER_OF_ROBOTS];
    int robotpipes[NUMBER_OF_ROBOTS * 2];
    int logpipe[2];
    int newpipe[2];
    int parentpipe[2];
    int pipes_count = 0;
    string robotupdate;
    pid_t robot_pids[NUMBER_OF_ROBOTS];
    pid_t robotPID;
    pid_t logPID;
    pid_t parent = ::getpid();
    printf ("Parent is %d, num children = %d\n", parent, NUMBER_OF_ROBOTS);

    //Check for cmdfile, getting input from user if not found
    util_funcs::checkForCommandFile(argv, argc, cmdfilename, log1, cmdfile, line, cmdline_commands);

    //Process Command instructions building a vector for each Child/Robot
    util_funcs::processCommandInstructions(log1, cmdfile, cmdline_commands, line, robotcommands, NUMBER_OF_ROBOTS);

    //Create the parent pipe for the robots to send status updates through
    //and the log pipe for the parent to send updates through so they can be logged
    pipe(parentpipe);
    pipe(logpipe);

    //Create the pipes for the robots, building an array where:
    // (robot number -1) * 2 is the read end and and ((robot number - 1) * 2) + 1 is the write end
    while (pipes_count < (NUMBER_OF_ROBOTS * 2)) {
        // Pipe creation
        pipe(newpipe);
        printf ("creating robotpipe %d\n", pipes_count/2);
        robotpipes[pipes_count++] = newpipe[0];  // Just the read end of the pipe
        robotpipes[pipes_count++] = newpipe[1]; // The write end
    }

    if(parent == ::getpid()) {
      //Write all robot commands from robotcommands
      for (int i = 0; i < NUMBER_OF_ROBOTS; i++) {
        for (int j = 0; j < robotcommands[i].size(); j++) {
            msg.from = j;
            strcpy(msg.payload, robotcommands[i][j].c_str());
            msg.payload[strlen(msg.payload)] = '\0';

            write(robotpipes[i*2 + 1], (void*)&msg, sizeof(msg));
            //printf("Wrote to pipe #%d this: %s \n", i, msg.payload);
        }
        close(robotpipes[i*2 + 1]);      // EOF to child
      }
      robotPID = parent;
      //Create the robot processes, storing their pid's for parent to wait on
      for (int i = 0 ; i < NUMBER_OF_ROBOTS ; i++)  {
        if (robotPID < 0) {
          perror ("Unable to fork");
          exit(1);
        } else if (robotPID != 0) {
            robotPID = fork();         //Creates our next child process
            //Child code
            if(robotPID == 0) {
              robot_pids[i] = ::getpid();   //Store the pid so we know which child we have for pipe reads
              printf ("creating Robot %d, PID: %d\n", (i + 1), robot_pids[i]);
              robots[i] = Robot(board);     //Create the robot from the current board
            } else {
              robot_pids[i] = robotPID;
            }
        }
      }
    }

    //Child code for reading/processing commands. Loop through robots based on pid, and read
    //from the corresponding pipe, process the move, and send updated position to parent for logging
    for(int i = 0; i < NUMBER_OF_ROBOTS; i++) {
      //Child code
      if(robot_pids[i] == ::getpid()) {
        close(robotpipes[i*2 + 1]);    // close unused write end
        close(parentpipe[0]);          // close unused read end of parent pipe

        //Read all commands from pipe
        while(read(robotpipes[i*2], (void*)&msg, sizeof(Message)) > 0) {
            printf("Robot #%d received: %s\n", (i + 1), msg.payload);
            //Take the direction from the end of command and pass to getPosition robot method
            robots[i].updatePosition(msg.payload[strlen(msg.payload) -1]);
            int *position = robots[i].getPosition();
            //Build update string and write to parent pipe
            robotupdate = "P " + std::to_string(i + 1) + " " + std::to_string(position[0]) + " " + std::to_string(position[1]);
            strcpy(msg.payload, robotupdate.c_str());
            msg.payload[strlen(msg.payload)] = '\0';
            write(parentpipe[1], (void*)&msg, sizeof(msg));
        }
        close(robotpipes[i*2]);    // close read end
      }
    }

    //Parent Code
    if(parent == ::getpid()) {
      //Close unused write end of parentpipe
      close(parentpipe[1]);
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
        //Loop through parents pipe and pass on to logging process
        while(read(parentpipe[0], (void*)&msg, sizeof(Message)) > 0) {
            printf("Parent received: %s\n", msg.payload);
            msg.payload[strlen(msg.payload)] = '\0';
            write(logpipe[1], (void*)&msg, sizeof(msg));
        }
        //We're done logging, so close write end of logpipe
        close(logpipe[1]);
        //Wait on children to die...
        for (int i = 0; i < NUMBER_OF_ROBOTS; i++) {
          cout << "Waiting for PID: " << robot_pids[i] << " to finish" << endl;
          waitpid(robot_pids[i], NULL, 0);
          cout << "PID: " << robot_pids[i] << " has shut down" << endl;
        }
        cout << "Waiting for Log: " << logPID << " to finish" << endl;
        waitpid(logPID, NULL, 0);
        cout << "Log: " << logPID << " has shut down" << endl;
      }
    }
    return 0;
}
