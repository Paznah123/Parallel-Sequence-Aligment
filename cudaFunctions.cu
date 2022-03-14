#include <cuda_runtime.h>
#include <helper_cuda.h>

#include "header.h"

// ======================== macros for allocating device memory and error check

#define CUDA_ERR_CHECK(err,msg) (\
		{if (err != cudaSuccess) { \
			fprintf(stderr, msg " - %s\n", cudaGetErrorString(err)); \
			exit(EXIT_FAILURE); \
		} \
	})

#define CUDA_MEM_INIT(dest, src, size, type) {\
	cudaError_t err = cudaSuccess;\
	size_t  arrSize = size * sizeof(type);\
	err = cudaMalloc((void**)&dest, arrSize);\
	CUDA_ERR_CHECK(err, "Failed to allocate device memory");\
	err = cudaMemcpy(dest, src, arrSize, cudaMemcpyHostToDevice);\
	CUDA_ERR_CHECK(err, "Failed to copy data from host to device"); }\

// ======================== calculate mutant score

__device__ float calcMutantScore(char* seq1, char* seq2, float* ABCgrid, int len2, int n, int k)
{
	float score = 0;
	int i = 0, j = 0;
	
	for (; j < len2; i++, j++)
	{
		if (j == n || j == k) 
			j++;
		int fc_idx = seq1[i] - 'A';
		int sc_idx = seq2[j] - 'A';
		score += ABCgrid[fc_idx*ABC_NUM + sc_idx];
	}	

 	return score;	
}

// ======================== find mutant best score

__global__ void calcMutantBestScoreKernel(char* d_seq1, char* d_seq2, float* d_bestScores, int* d_bestOffsets, 
						int* d_nkArr, float* d_ABCgrid, int num_mutants, int maxOffset, int len2)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	int offset = 0;
	float bestScore = -10000000000.0;
	
	if (i < num_mutants)
	{
		for (int j = 0; j <= maxOffset; j++)
		{
			float score = calcMutantScore(d_seq1 + j, d_seq2, d_ABCgrid, len2, d_nkArr[i], d_nkArr[i+num_mutants]);
			if (score > bestScore)
			{
				bestScore = score;
				offset = j;
			}
		}
		d_bestScores[i] = bestScore;
		d_bestOffsets[i] = offset;
	}

}

// ======================== entry point to CUDA

void calcBestScoreCUDA(char* seq1, char* seq2, Data data)
{
	int len1 = strlen(seq1);
	int maxOffset = len1 - (data.len2-2);

	char* d_seq1 = NULL; // allocate seq1 memory
	CUDA_MEM_INIT(d_seq1, seq1, len1, char);
	
	char* d_seq2 = NULL; // allocate seq2 memory
	CUDA_MEM_INIT(d_seq2, seq2, data.len2, char);
	
	int* d_nkArr = NULL; // allocate nk array memory
	CUDA_MEM_INIT(d_nkArr, data.nkArr, data.num_mutants*2, int);

	float* d_ABCgrid = NULL; // allocate ABC grid memory
	CUDA_MEM_INIT(d_ABCgrid, data.ABCgrid, ABC_NUM*ABC_NUM, float);
	
	float* d_bestScores = NULL;  // allocate best scores memory
	CUDA_MEM_INIT(d_bestScores, data.bestScores, data.num_mutants, float);

	int* d_bestOffsets = NULL;  // allocate best offsets memory
	CUDA_MEM_INIT(d_bestOffsets, data.bestOffsets, data.num_mutants, int);
	
	int threads = 32;
	int blocks = (data.num_mutants + threads-1) / threads;
	
	calcMutantBestScoreKernel<<<blocks, threads>>>(d_seq1, d_seq2, d_bestScores, d_bestOffsets, d_nkArr, d_ABCgrid, data.num_mutants, maxOffset, data.len2);
	
	// copy results to host
	cudaMemcpy(data.bestScores, d_bestScores, data.num_mutants * sizeof(float), cudaMemcpyDeviceToHost);
	cudaMemcpy(data.bestOffsets, d_bestOffsets, data.num_mutants * sizeof(int), cudaMemcpyDeviceToHost);
	
	cudaFree(d_seq1);
	cudaFree(d_seq2);
	cudaFree(d_bestScores);
	cudaFree(d_bestOffsets);
	cudaFree(d_nkArr);
	cudaFree(d_ABCgrid);
}

// ========================

