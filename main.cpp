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

    const int NUMBER_OF_CHILDREN = board.numrobots;
    std::vector<string> cmdline_commands;
    std::vector<string> robotcommands[NUMBER_OF_CHILDREN];
    Message msg{0, "Blagaga"};
    Robot robots[NUMBER_OF_CHILDREN];
    int childpipes[NUMBER_OF_CHILDREN * 2];
    int newpipe[2], status;
    int pipes_count = 0;
    pid_t shut_down[NUMBER_OF_CHILDREN];
    pid_t child;
    pid_t parent = ::getpid();
    printf ("Parent is %d, num children = %d\n", parent, NUMBER_OF_CHILDREN);

    //Check for cmdfile, getting input from user if not found
    util_funcs::checkForCommandFile(argv, argc, cmdfilename, log1, cmdfile, line, cmdline_commands);

    //Process Command processSetupInstructions
    util_funcs::processCommandInstructions(log1, cmdfile, cmdline_commands, line, robotcommands, NUMBER_OF_CHILDREN);

    //Create the pipes
    while (pipes_count < (NUMBER_OF_CHILDREN * 2)) {
        // Pipe creation
        pipe(newpipe);
        printf ("creating pipe %d\n", pipes_count/2);
        childpipes[pipes_count++] = newpipe[0];  // Just the read end of the pipe
        childpipes[pipes_count++] = newpipe[1]; // The write end

    }

    //Write all robot commands from robotcommands
    for (int i = 0; i < NUMBER_OF_CHILDREN; i++) {
      for (int j = 0; j < robotcommands[i].size(); j++) {
          msg.from = j;
          strcpy(msg.payload, robotcommands[i][j].c_str());
          msg.payload[strlen(msg.payload)] = '\0';

          write(childpipes[i*2 + 1], (void*)&msg, sizeof(msg));
          printf("Wrote to pipe #%d this: %s \n", i, msg.payload);
      }
      close(childpipes[i*2 + 1]);      // EOF to child
    }

    //Create the children
    child = fork();
    if(child == 0) {
      //printf ("storing child %d\n", getpid());
      shut_down[0] = getpid();   //Store the pid so we can wait on it later
    } else {
      shut_down[0] = child;
    }
    for (int i = 1 ; i < NUMBER_OF_CHILDREN ; i++)  {
      if (child < 0) {
        perror ("Unable to fork");
        exit(1);
      } else if (child != 0) {
          //printf ("creating child %d\n", i);
          child = fork();         //Creates our next child process
          if(child == 0) {
            //printf ("storing child %d\n", getpid());
            shut_down[i] = getpid();   //Store the pid so we can wait on it later
            robots[i] = Robot(board);     //Create a robot for the child
          } else {
            shut_down[i] = child;
          }
    }

    }

    for(int i = 0; i < NUMBER_OF_CHILDREN; i++) {
      // child code
       if(shut_down[i] == getpid()) {
        //printf("Robot %d Attempting read from pipe %d\n", ::getpid(), i);
        close(childpipes[i*2 + 1]);    // close unused write end

        //int *position = robots[i].getPosition();
        //cout << std::to_string(position[0]) << "   " << std::to_string(position[1]) << "\n";

        while(read(childpipes[i*2], (void*)&msg, sizeof(Message)) > 0) {
            printf("Robot #%d: %s\n", ::getpid(), msg.payload);
        }
        close(childpipes[i*2]);    // close read end
      } else {
        //printf("%d != %d when i = %d\n", shut_down[i], getpid(), i);
      }
    }

    //Wait on children to die...
    if(parent == ::getpid()) {
      for (int i = 0; i < NUMBER_OF_CHILDREN; i++){
        cout << "Waiting for PID: " << shut_down[i] << " to finish" << endl;
        waitpid(shut_down[i], NULL, 0);
        cout << "PID: " << shut_down[i] << " has shut down" << endl;
      }
    }
    return 0;
}
