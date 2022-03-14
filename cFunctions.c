#include "header.h"
#include "omp.h"
#include <float.h>

Data initData(int len2, float weights[])
{
	Data data;
	
	data.len2 = len2;
	data.num_mutants = len2 * (len2 - 1) / 2;
	
	data.nkArr = createNK(data.num_mutants);
	data.ABCgrid = createABCgrid(weights);
	
	data.bestScores = (float*)calloc(data.num_mutants, sizeof(float));	
	data.bestOffsets = (int*)calloc(data.num_mutants, sizeof(int));
	
	return data;
}

// ======================== main work function

Result findBestMutant(char* seq1, char* seq2, float weights[])
{
	Data data = initData(strlen(seq2), weights);	

	calcBestScoreCUDA(seq1, seq2, data);

	Result res = findBestMutantAndOffsetOMP(data);
	
	return res;
}


// ======================== find best mutant from all seq2 mutants

Result findBestMutantAndOffsetOMP(Data data)
{
	Result res;
	res.score = -FLT_MAX;
	
	#pragma omp parallel for
	for(int i = 0; i < data.num_mutants; i++) // find best score of all mutants
	{
		if (data.bestScores[i] > res.score)
		{
			res.score = data.bestScores[i]; 
			res.offset = data.bestOffsets[i];
			res.n = data.nkArr[i] + 1;
			res.k = data.nkArr[i+data.num_mutants] + 1;
		}
	}

	return res;
}

// ========================

