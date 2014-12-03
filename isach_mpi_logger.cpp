#include "isach_mpi_logger.h"
#include <unistd.h>

namespace isach_mpi {

std::vector<Logger*> Logger::_loggers(5);

    Logger::Logger(char* logFileName) {
        _logFile = fopen(logFileName, "wa");
    }

    void Logger::closeLogFile() {
        fclose(_logFile);
    }

    Logger* Logger::getLogger(int procRank) {
        return Logger::_loggers[procRank];
    }

    void Logger::addLogger(int procRank, char* logFileName) {
        _loggers[procRank] = new Logger(logFileName);
    }

    void Logger::closeLogger(int procRank)
    {
        delete _loggers[procRank];
        _loggers[procRank] = nullptr;
    }

    void Logger::log(const char* msg)
    {
        fprintf(_logFile, "%s", msg);
        int fileDesc = fileno(_logFile);
        syncfs(fileDesc);
    }

    void Logger::log(const int msg)
    {
        fprintf(_logFile, "%d", msg);
        int fileDesc = fileno(_logFile);
        syncfs(fileDesc);

    }

    Logger::~Logger() {
        closeLogFile();
    }
}

