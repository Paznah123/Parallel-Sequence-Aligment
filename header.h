#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===================== macros

#define NUM_PROCS 2

#define MASTER 0
#define SLAVE 1

#define READ_FILE_NAME "input.txt"
#define WRITE_FILE_NAME "output.txt"

#define SEQ1_LEN 5000
#define SEQ2_LEN 3000

#define WEIGHTS_NUM 4
#define MAX_SEQS2 30
#define ABC_NUM 26

#define CONSERVATIVES_LEN 9
#define SEMI_CONSERVATIVES_LEN 11

// ===================== 

typedef struct
{
	float score;
	int offset, n, k;
	int mutant_num;
} Result;

typedef struct
{
	int len2;
	int num_mutants;
	int* nkArr;
	float* ABCgrid;
	float* bestScores;
	int* bestOffsets;
	
} Data;

// ===================== fileUtil

char** readFromFile(const char* fileName, float weights[], char* seq1, int* numOfSeq2);
void writeToFile(const char* fileName, Result* results, int num_seqs);

// ===================== helpers

int* createNK(int num_mutants);
float* createABCgrid(float weights[]);
float compareChars(char first, char second, float weights[]);
int checkInGroup(char first, char second, const char** group, int len);

// ===================== cFunctions

Result findBestMutant(char* seq1, char* seq2, float weights[]);
Result findBestMutantAndOffsetOMP(Data data);
Data initData(int len2, float weights[]);

// ===================== cudaFunctions

void calcBestScoreCUDA(char* seq1, char* seq2, Data data);


