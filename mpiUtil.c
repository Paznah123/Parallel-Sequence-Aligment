#include "mpiUtil.h"
#include <stddef.h>

#define RESULT_ATTRS_NUM 5

// ======================== initialize mpi communication

void InitMPI(int* argc, char** argv[], int* rank, int* num_procs, MPI_Datatype* resultType)
{
	MPI_Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, rank); 
	MPI_Comm_size(MPI_COMM_WORLD, num_procs); 

	createResultType(resultType); 
}

// ======================== create data type for mpi communication

void createResultType(MPI_Datatype* resultType)
{
	int blockLen[RESULT_ATTRS_NUM] = { 1, 1, 1, 1, 1 };
	MPI_Aint disp[RESULT_ATTRS_NUM];
	MPI_Datatype types[RESULT_ATTRS_NUM] = { MPI_FLOAT, MPI_INT, MPI_INT, MPI_INT, MPI_INT };

	disp[0] = offsetof(Result, score);
	disp[1] = offsetof(Result, offset);
	disp[2] = offsetof(Result, n);
	disp[3] = offsetof(Result, k);
	disp[4] = offsetof(Result, mutant_num);
	
	MPI_Type_create_struct(RESULT_ATTRS_NUM, blockLen, disp, types, resultType);
	MPI_Type_commit(resultType);
}

// ========================

void sendSlaveData(char* seq1, char** seq2s, int numSeq2s, float weights[])
{
	int len = strlen(seq1);
	MPI_Send(&len, 1, MPI_INT, SLAVE, 0, MPI_COMM_WORLD);	
	MPI_Send(seq1, len, MPI_CHAR, SLAVE, 0, MPI_COMM_WORLD);
	MPI_Send(&numSeq2s, 1, MPI_INT, SLAVE, 0, MPI_COMM_WORLD);

	for (int i = 0; i < numSeq2s; i++)	
	{
		len = strlen(seq2s[i]);
		MPI_Send(&len, 1, MPI_INT, SLAVE, 0, MPI_COMM_WORLD);
		MPI_Send(seq2s[i], len, MPI_CHAR, SLAVE, 0, MPI_COMM_WORLD);
	}
	
	MPI_Send(weights, WEIGHTS_NUM, MPI_FLOAT, SLAVE, 0, MPI_COMM_WORLD);
}

// ========================

char** recvMasterData(char* seq1, int* numSeq2s, float weights[])
{
	MPI_Status status;
	int len;
	
	MPI_Recv(&len, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &status);
	MPI_Recv(seq1, len, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, &status);
	MPI_Recv(numSeq2s, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &status);
	
	char** seq2s = (char**) calloc(*numSeq2s, sizeof(char*));
	for (int i = 0; i < *numSeq2s; i++)	
	{
		MPI_Recv(&len, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &status);
		seq2s[i] = (char*) calloc(len, sizeof(char));
		MPI_Recv(seq2s[i], len, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, &status);
	}

	MPI_Recv(weights, WEIGHTS_NUM, MPI_FLOAT, MASTER, 0, MPI_COMM_WORLD, &status);
	
	return seq2s;
}

// ========================



