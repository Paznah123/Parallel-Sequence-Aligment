#pragma once

#include "mpi.h"
#include "header.h"

// ========================

void InitMPI(int* argc, char** argv[], int* rank, int* num_procs, MPI_Datatype* resultType);
void createResultType(MPI_Datatype* resultType);
void sendSlaveData(char* seq1, char** seq2s, int numSeq2s, float weights[]);
char**  recvMasterData(char* seq1, int* numSeq2s, float weights[]);

// ========================

