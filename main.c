#include "mpi.h"
#include "omp.h"
#include "mpiUtil.h"

// ========================

int main(int argc, char* argv[])
{
	int rank, num_procs, numSeq2s;
	char **seq2s, seq1[SEQ1_LEN];
	float weights[WEIGHTS_NUM];	
	Result* results;
	
	MPI_Datatype MPI_RESULT;
	MPI_Status status;

	int start_time = omp_get_wtime();
	
	InitMPI(&argc, &argv, &rank, &num_procs, &MPI_RESULT);

	if (num_procs != NUM_PROCS)
	{
		printf("please use this program with %d processes\n", NUM_PROCS);
		MPI_Abort(MPI_COMM_WORLD, __LINE__);
	}
	
	if (rank == MASTER) 
	{
		seq2s = readFromFile(READ_FILE_NAME, weights, seq1, &numSeq2s); // get input data	

		results = (Result*) calloc(numSeq2s, sizeof(Result)); // allocate memory for results

		sendSlaveData(seq1, seq2s + numSeq2s/NUM_PROCS, numSeq2s/NUM_PROCS, weights);

		for (int i = 0; i < numSeq2s/NUM_PROCS; i++) // calculate best results
			results[i] = findBestMutant(seq1, seq2s[i], weights);
			
		printf("process %d finished calculations\n", rank);
		
		for (int i = numSeq2s/NUM_PROCS; i < numSeq2s; i++) // recieve slave results
			MPI_Recv(&results[i], 1, MPI_RESULT, SLAVE, 0, MPI_COMM_WORLD, &status);
		
		printf("process %d recieved results\n", rank);
			
		writeToFile(WRITE_FILE_NAME, results, numSeq2s); // write results to file	
	}
	else 
	{	
	
		seq2s = recvMasterData(seq1, &numSeq2s, weights);
		
		results = (Result*) calloc(numSeq2s, sizeof(Result)); // allocate memory for results

		for (int i = 0; i < numSeq2s; i++) // calculate best results
			results[i] = findBestMutant(seq1, seq2s[i], weights);
		
		printf("process %d finished calculations\n", rank);
		
		for (int i = 0; i < numSeq2s; i++) // send to master best results
			MPI_Send(&results[i], 1, MPI_RESULT, MASTER, 0, MPI_COMM_WORLD);
			
		printf("process %d sent results\n", rank);
	}
	
	for (int i = 0; i < numSeq2s; i++)
		free(seq2s[i]);
	free(seq2s);
	free(results);
	
	printf("process %d finished work after - %.2f seconds\n", rank, omp_get_wtime() - start_time);	
	
	MPI_Finalize();
	return 0;
}

// ========================

