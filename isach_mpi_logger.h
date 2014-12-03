#ifndef ISACH_MPI_LOGGER_H
#define ISACH_MPI_LOGGER_H

#include <vector>
#include <iostream>


namespace isach_mpi {

class Logger{
private:
    FILE* _logFile;
    static std::vector<Logger*> _loggers;
    Logger(char* logFileName);
    void closeLogFile();

public:

    static Logger* getLogger(int procRank);

    static void addLogger(int procRank, char* logFileName);

    static void closeLogger(int procRank);

    void log(const char* msg);
    void log(const int msg);

    ~Logger();
};

}

#endif // ISACH_MPI_LOGGER_H
