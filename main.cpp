#include <stdio.h>
#include <mpich/mpi.h>
#include <getopt.h>
#include <string.h>
#include <isach_mpi_readMatrixFromFIle.h>
#include <isach_mpi_transposition.h>
#include <isach_mpi_logger.h>
#include <stdlib.h>
#include <signal.h>

#define COLOR_RESET "\e[m"
#define COLOR_GREEN "\e[32m"
#define COLOR_RED  "\033[22;31m"

#define MASTER_NUM 0

int rank, size;
int vectorSize;
int* vector;
int n;
int m;
int *matrix_part;


void segfault_sigaction(int signal, siginfo_t *si, void *arg)
{
    printf("Caught segfault at address %p\n", si->si_addr);
    isach_mpi::Logger::closeLogger(rank);
    exit(0);
}


void output(char *ansFileName, int* ans)
{
}



int main (int argc, char **argv)
{
    MPI_Init (&argc, &argv);	/* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

    char logFileName[30];
    sprintf(logFileName, "log/mpi_lab3-proc%d.log", rank);
    isach_mpi::Logger::addLogger(rank, logFileName);
    isach_mpi::Logger::getLogger(rank)->log("start\n");

    char* fileName = argv[1];

    isach_mpi::read_matrix_from_file(fileName, MPI_COMM_WORLD, &n, &m, &matrix_part);
    isach_mpi::Logger::getLogger(rank)->log("matrix got\n");

    isach_mpi::Logger::getLogger(rank)->log("Before:\n");
    for(int i = 0; i < n / 2; ++i) {
        for(int j = 0; j < m; ++j) {
            char logmsg[20];
            sprintf(logmsg, "%d ", matrix_part[i*m + j]);
            isach_mpi::Logger::getLogger(rank)->log(logmsg);
        }
        isach_mpi::Logger::getLogger(rank)->log("\n");
    }

    isach_mpi::transposition(&matrix_part, n, m, MPI_COMM_WORLD);


    isach_mpi::Logger::getLogger(rank)->log("After:\n");
    for(int i = 0; i < m ; ++i) {
        for(int j = 0; j < n; ++j) {
            char logmsg[20];
            sprintf(logmsg, "%d ", matrix_part[i*n + j]);
            isach_mpi::Logger::getLogger(rank)->log(logmsg);
        }
        isach_mpi::Logger::getLogger(rank)->log("\n");
    }

    MPI_Finalize();
    return 0;
}
