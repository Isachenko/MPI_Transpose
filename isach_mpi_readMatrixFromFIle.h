#ifndef MY_MPI_READMATRIXFROMFILE_H
#define MY_MPI_READMATRIXFROMFILE_H

#include <mpich/mpi.h>
#include <stdio.h>
#include <QFile>
#include <QTextStream>
#include <isach_mpi_logger.h>

namespace isach_mpi {

#define TAG_SCATTER_FROM_FILE 99

void get_next_part(FILE* matrixFile, int partSize, int rowSize, int* buf)
{
    char logmsg[20];
    sprintf(logmsg, "partSize: %d\n", partSize);
    isach_mpi::Logger::getLogger(0)->log(logmsg);
    sprintf(logmsg, "rowSize: %d\n", rowSize);
    isach_mpi::Logger::getLogger(0)->log(logmsg);

    for(int i = 0; i < partSize*rowSize; ++i) {
        fscanf(matrixFile, "%d", buf + i);
        char logmsg[10];
        sprintf(logmsg, "%d, ", buf[i]);
        isach_mpi::Logger::getLogger(0)->log(logmsg);
    }
    printf("\n");
}


void read_matrix_from_file(char* file_name, MPI_Comm comm, int* n, int* m, int** matrix)
{
    int rank;
    int comm_size;
    int block_size;
    MPI_Comm comm_copy;
    MPI_Comm_dup(comm, &comm_copy);
    FILE* input_file;

    MPI_Comm_rank (comm_copy, &rank);	/* get current process id */
    MPI_Comm_size (comm_copy, &comm_size);	/* get number of processes */
    if (rank == 0) {
        input_file = fopen(file_name, "r");
        fscanf(input_file, "%d %d", n, m);
    }
    MPI_Bcast(n, 1, MPI_INT, 0, comm_copy);
    MPI_Bcast(m, 1, MPI_INT, 0, comm_copy);
    block_size = *n / comm_size + (rank < (*n % comm_size));

    // proces will read file and send part to other process
    if (rank == 0) {
        //first block for me
        *matrix = new int[block_size * (*m)];
        get_next_part(input_file, block_size, *m, *matrix);
        //other blocks for others
        int* tmpBuff;
        for (int  i = 1; i < comm_size; ++i) {
            int block_size_i = ((*n) / comm_size + (i < (*n) % comm_size));
            tmpBuff = new int[block_size_i * (*m)];
            get_next_part(input_file, block_size_i, *m, tmpBuff);
            MPI_Send(tmpBuff, block_size_i * (*m), MPI_INT, i, TAG_SCATTER_FROM_FILE, comm_copy);
        }
    }
    //proccess will wait for part from 0-proccess
    else {
        MPI_Status status;
        *matrix = new int[block_size * (*m)];
        MPI_Recv(*matrix, block_size * (*m), MPI_INT, 0, TAG_SCATTER_FROM_FILE, comm_copy, &status);
    }

    MPI_Comm_free(&comm_copy);
}

} //namespace isach_mpi

#endif // MY_MPI_READMATRIXFROMFILE_H
