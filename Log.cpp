#include "Log.h"
#include <iostream>
#include <ctime>
#include <fstream>

const string Log::DEFAULT_LOG_FILE_NAME = "log.txt";

Log::Log() {
    logfilename = Log::DEFAULT_LOG_FILE_NAME;
};

Log::Log(char* lname) {
    if((lname != NULL) && (lname[0] == '\0')) {
        logfilename = std::string(lname);
    }
}

Log::Log(string lname){
    logfilename = lname;
}

string Log::getTimeStamp() {
    time_t curr_time;
    curr_time = time(NULL);
    return ctime(&curr_time);
}

void Log::setLogfileName(string cname) {
    logfilename = cname;
}

string Log::getLogfileName() {
    return logfilename;
}

string Log::getDefaultLogfileName() {
    return Log::DEFAULT_LOG_FILE_NAME;
}

int Log::open() {
    std::ofstream logF(logfilename, std::ios_base::app);
    if(!logF.fail()) {
        logF << getTimeStamp() << "Begin\n";
        logF.close();
        return 1;
    } else {
        std::cout << "Error opening log file.";
        return 0;
    }
}

int Log::close() {
    std::ofstream logF(logfilename, std::ios_base::app);
    logF << getTimeStamp() << "End\n";
    return 1;
}

int Log::writeLogRecord(string s) {
    std::ofstream logF(logfilename, std::ios_base::app);
    //Append line to document
    logF << getTimeStamp() << s << std::endl;
    logF.close();
    return 1;
}