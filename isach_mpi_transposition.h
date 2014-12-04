#ifndef MY_MPI_TRANSPOSITION_H
#define MY_MPI_TRANSPOSITION_H

#include <mpich/mpi.h>
#include <algorithm>
#include "isach_mpi_logger.h"

namespace isach_mpi {

#define TAG_BLOCK 10


void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void transpose(int* fromBuf, int* toBuf, int n, int m, int strideN, int strideM, int rank) {
    //isach_mpi::Logger::getLogger(rank)->log("fromBuf: \n");

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            //isach_mpi::Logger::getLogger(rank)->log(" ");
            //isach_mpi::Logger::getLogger(rank)->log(fromBuf[j * strideN + i]);
            toBuf[i * strideM + j] = fromBuf[j * strideN + i];
        }
    }
    //isach_mpi::Logger::getLogger(rank)->log("\n");
}

int blockSize(int n, int i, int commsize) {
   return n / commsize + (i < (n % commsize));
}

void transposition(int** buf, int n, int m, MPI_Comm comm) {
    MPI_Comm newcomm;
    MPI_Comm_dup(comm, &newcomm);


    int rank;
    MPI_Comm_rank(newcomm, &rank);
    int commsize;
    MPI_Comm_size(newcomm, &commsize);

    for (int i = 0; i < commsize; i++) {
        if (i != rank) {
            int blockSizeN = blockSize(n, rank, commsize);
            int blockSizeM = blockSize(m, i, commsize);
            int smallBlockSizeM = m / commsize;
            int bigBlockCount = std::min(i, m % commsize);
            MPI_Datatype block;
            MPI_Type_vector(blockSizeN, blockSizeM, m, MPI_INT, &block);
            MPI_Type_commit(&block);

            //logend
            MPI_Send(*buf + i * smallBlockSizeM + bigBlockCount, 1, block, i, TAG_BLOCK, newcomm);

            MPI_Type_free(&block);
        }
    }

    swap(&n, &m);
    int blockSizeN = blockSize(n, rank, commsize);
    int* rBuf = new int[blockSizeN * m];
    for (int i = 0; i < commsize; i++) {
        int bigBlockCountM = std::min(i, m % commsize);
        int smallBlockSizeN = n / commsize;
        int smallBlockSizeM = m / commsize;
        int blockSizeM = blockSize(m, i, commsize);

        if (rank != i) {
            MPI_Datatype raw;
            MPI_Type_vector(blockSizeN, 1, m, MPI_INT, &raw);
            MPI_Type_commit(&raw);
            MPI_Datatype tmatrix;
            MPI_Type_hvector(blockSizeM, 1, sizeof(int), raw, &tmatrix);
            MPI_Type_commit(&tmatrix);

            MPI_Status status;
            MPI_Recv(rBuf + i * smallBlockSizeM + bigBlockCountM, 1, tmatrix, i, TAG_BLOCK, newcomm, &status);

            MPI_Type_free(&tmatrix);
            MPI_Type_free(&raw);
        }  else {
            int bigBlockCountN = std::min(i, n % commsize);
            int bigBlockCountM = std::min(i, m % commsize);
            transpose(*buf + rank * smallBlockSizeN + bigBlockCountN, rBuf + rank * smallBlockSizeM + bigBlockCountM, blockSizeN, blockSizeM, n, m, rank);
        }
    }
    delete *buf;
    *buf = rBuf;

    MPI_Comm_free(&newcomm);
}

} //namespace isach_mpi






#endif // MY_MPI_TRANSPOSITION_H
