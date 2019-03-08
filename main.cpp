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
#include "utilities.cpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct Message
{
    int from;
    char payload[32];
};

int main(int argc, char** argv) {

    Log log1;
    string setupfilename, cmdfilename, line;
    std::vector<string> commands;
    std::ifstream setupfile, cmdfile;
    Message msg{0, "Blagaga"};
    int file_desc[2], status;
    pid_t pid, pid2;

    //If log file name is present in command line arguments, set the logfilename for log1
    util_funcs::checkForLogFileAndSetLogFileName(argv, argc, log1);

    //Check for and open setupfile, if return -1 something went wrong so exit.
    if(util_funcs::checkForSetupFile(argv, argc, setupfilename, log1, setupfile) == -1) {
        return 0;
    }

    //Process Setup instructions
    util_funcs::processSetupInstructions(log1, setupfile, line);

    //Check for cmdfile, getting input from user if not found
    util_funcs::checkForCommandFile(argv, argc, cmdfilename, log1, cmdfile, line, commands);

    //Process Command processSetupInstructions
    util_funcs::processCommandInstructions(log1, cmdfile, commands, line);


    if (pipe(file_desc) == -1)
    {
        perror("pipe() failed. Exiting...");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    pid2 = fork();

    if (pid == -1)
    {
        perror("fork() failed. Exiting...");
        exit(EXIT_FAILURE);
    }

    // child code
    if (pid == 0)
    {
        close(file_desc[1]);    // close unused write end

        while(read(file_desc[0], (void*)&msg, sizeof(Message)) > 0)
            printf("Robot #%d: %s\n", msg.from, msg.payload);

        close(file_desc[0]);    // close read end
        _exit(EXIT_SUCCESS);    // exit IMMEDIATELY
    }
    if (pid2 == 0) {
      printf("%d\n", getpid());
    }
    // parent code
    else
    {
        close(file_desc[0]);    // close unused read end

        // write all messages
        for (int i = 1; i < argc; ++i)
        {
            msg.from = i;
            if(strlen(argv[i]) < sizeof(msg.payload))
            {
                strcpy(msg.payload, argv[i]);
                msg.payload[strlen(msg.payload)] = '\0';
            }
            else
                strcpy(msg.payload, "Argument too long!");

            write(file_desc[1], (void*)&msg, sizeof(msg));
        }
        close(file_desc[1]);    // EOF to child
        wait(&status);          // wait on child to terminate
        exit(EXIT_SUCCESS);
    }

    return 0;
}
