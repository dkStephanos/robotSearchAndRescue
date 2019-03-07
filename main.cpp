#include "Log.h"
#include "utilities.cpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>

int main(int argc, char** argv) {

    Log log1;
    string setupfilename, cmdfilename, line;
    std::vector<string> commands;
    std::ifstream setupfile, cmdfile;

    //If log file name is present in command line arguments, set the logfilename for log1
    util_funcs::checkForLogFileAndSetLogFileName(argv, argc, log1);

    log1.writeLogRecord(std::to_string(argc));
    //Check for and open setupfile, if return -1 something went wrong so exit.
    if(util_funcs::checkForSetupFile(argv, argc, setupfilename, log1, setupfile) == -1) {
        return 0;
    }

    //Check for cmdfile, getting input from user if not found
    util_funcs::checkForCommandFile(argv, argc, cmdfilename, log1, cmdfile, line, commands);

    //Pass in the log obect, our setup/command files, the command vector in case we took in user input and let log do its thing
    util_funcs::processSetupAndCommandInstructions(log1, setupfile, cmdfile, commands, line);

    return 0;
}
