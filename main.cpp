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
    int logpipe[2];
    int pipes_count = 0;
    string robotupdate;
    pid_t logPID;
    pid_t parent = ::getpid();
    printf ("Parent is %d, num children = %d\n", parent, NUMBER_OF_ROBOTS);

    //Check for cmdfile, getting input from user if not found
    util_funcs::checkForCommandFile(argv, argc, cmdfilename, log1, cmdfile, line, cmdline_commands);

    //Process Command instructions building a vector for each Child/Robot
    util_funcs::processCommandInstructions(log1, cmdfile, cmdline_commands, line, robotcommands, NUMBER_OF_ROBOTS);

    //Create log pipe for the parent to send updates through so they can be logged
    pipe(logpipe);

    //Parent code
    if(true) {
      //Write all robot commands from robotcommands
      for (int i = 0; i < NUMBER_OF_ROBOTS; i++) {

      }
      //Create the robot processes, storing their pid's for parent to wait on
      for (int i = 0 ; i < NUMBER_OF_ROBOTS ; i++)  {

      }
    }

    //Child code for reading/processing commands. Loop through robots based on pid, and read
    //from the corresponding pipe, process the move, and send updated position to parent for logging
    for(int i = 0; i < NUMBER_OF_ROBOTS; i++) {
      //Child code

        //Read all commands from queue
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
        //Loop through parents pipe and pass on to logging process
        while(false) {
            printf("Parent received: %s\n", msg.payload);
            msg.payload[strlen(msg.payload)] = '\0';
            write(logpipe[1], (void*)&msg, sizeof(msg));
        }
        //We're done logging, so close write end of logpipe
        close(logpipe[1]);
        //Wait on threads to join...
        for (int i = 0; i < NUMBER_OF_ROBOTS; i++) {
          cout << "Waiting for thread: to finish" << endl;
          cout << "Thread: has shut down" << endl;
        }
        cout << "Waiting for Log: " << logPID << " to finish" << endl;
        waitpid(logPID, NULL, 0);
        cout << "Log: " << logPID << " has shut down" << endl;
      }
    }
    return 0;
}
