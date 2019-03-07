/**
 * ---------------------------------------------------------------------------
 * File name: utilities.cpp
 * Project name: Project 3
 * Purpose: Provides utility functions primarily for parsing command line arguments.
 * ---------------------------------------------------------------------------
 * Creator's name and email: Koi Stephanos, stephanos@etsu.edu
 * Course:  Operating Systems
 * Creation Date: 2/12/2019
 * ---------------------------------------------------------------------------
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "Log.h"

namespace util_funcs {
    char* getCmdOption(char ** begin, char ** end, const std::string & option)
    {
        char ** itr = std::find(begin, end, option);
        if (itr != end && ++itr != end)
        {
            return *itr;
        }
        return 0;
    };

    bool cmdOptionExists(char** begin, char** end, const std::string& option)
    {
        char** arg = std::find(begin, end, option);
        return arg != end && (end - arg) > 1;
    };

    int checkForSetupFile(char** argv, int argc, string setupfilename, Log& log1, std::ifstream& setupfile) {
        //Check for setup file name in command line arguments, opening file if there, displaying error and exiting if not
        if(util_funcs::cmdOptionExists(argv, argv + argc, "-s")) {
            setupfilename = util_funcs::getCmdOption(argv, argv + argc, "-s");
            setupfile.open(setupfilename);
            //If file opening fails, log and display error
            if(setupfile.fail()) {
                string errormessage = "Setup file " + setupfilename + " does not exist.";
                log1.writeLogRecord(errormessage);
                log1.writeLogRecord("End");
                cout << errormessage + "\n";
                return -1;
            }
        } else {
            std::cout << "Setup file required. Please try again and indicate setup file with the -s flag.\n";
            return -1;
        }
        //If we get this far, everything went okay so return 1
        return 1;
    }

    int checkForCommandFile(char** argv, int argc, string cmdfilename, Log& log1, std::ifstream& cmdfile, string line, std::vector<string> &commands) {
        //Check for command file name in command line arguments, opening if file is there, prompting for commands from the keyboard if not
        if(util_funcs::cmdOptionExists(argv, argv + argc, "-c")) {
            cmdfilename = util_funcs::getCmdOption(argv, argv + argc, "-c");
            cmdfile.open(cmdfilename);

            if(cmdfile.fail()) {
                string errormessage = "Command file " + cmdfilename + " does not exist.";
                log1.writeLogRecord(errormessage);
                log1.writeLogRecord("End");
                cout << errormessage + "\n";
                return -1;
            }
            //Once we open the file, return 1
            return 1;

        } else {
            std::cout << "Command file not found.\nPlease enter commands:\n";

            while(line != "Q") {
                std::getline(std::cin, line);
                commands.push_back(line);
            }
            //When done getting input, return 1
            return 1;
        }
        //If we get this far something went wrong so return -1
        return -1;
    }

    int checkForLogFileAndSetLogFileName(char** argv, int argc, Log& log1) {
        if(util_funcs::cmdOptionExists(argv, argv + argc, "-l")) {
            log1.setLogfileName(util_funcs::getCmdOption(argv, argv + argc, "-l"));
            return 1;
        }
        //If no log file indicated, return -1 so we know we're using the default
        return -1;
    }

    int processSetupAndCommandInstructions(Log& log1, std::ifstream& setupfile, std::ifstream& cmdfile, std::vector<string> &commands, string line) {

        //If result of open is 0, something went wrong, so we exit, otherwise, loop through setup and command lines, appending to log
        if(log1.open() == 0) {
            return 0;
        } else {
            //First loop through setup instructions
            while (std::getline(setupfile, line))
            {
                log1.writeLogRecord(line);
            }
            //Next loop through command instructions, using the commands vector if populated, otherwise reading in from cmdfile
            if(commands.size() > 0) {
                for(int i = 0; i < commands.size(); i++) {
                    log1.writeLogRecord(commands[i]);
                }
            } else {
                while (std::getline(cmdfile, line))
                {
                    log1.writeLogRecord(line);
                }
            }

            //Finally, close log1 appending final timestamp and "End"
            log1.close();
        }

        //If we get this far, we're good, so return 1
        return 1;
    }
}
