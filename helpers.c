#include "header.h"

// ======================== groups

const char* conservatives[CONSERVATIVES_LEN] = {
	"NDEQ", "NEQK", "STA", "MILV", "QHRK", "NHQK", "FYW", "HY", "MILF"
};

const char* semi_conservatives[SEMI_CONSERVATIVES_LEN] = {
	"SAG", "ATV", "CSA", "SGND", "STPA", "STNK", "NEQHRK", "NDEQHK", "SNDEQK", "HFY", "FVLIM"
};

// ======================== checks if both chars in the group

int checkInGroup(char first, char second, const char** group, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (strchr(group[i], first) != NULL
			&& strchr(group[i], second) != NULL)
			return 1;
	}
	return 0;
}

// ======================== assign suitable weight for chars 

float compareChars(char first, char second, float weights[])
{
	if (first == second)
		return weights[0];
	else if (checkInGroup(first, second, conservatives, CONSERVATIVES_LEN))
		return -weights[1];
	else if (checkInGroup(first, second, semi_conservatives, SEMI_CONSERVATIVES_LEN))
		return -weights[2];

	return -weights[3];
}

// ======================== creates alphabet grid for char comparison

float* createABCgrid(float weights[])
{
	float* ABCgrid = (float*)malloc(ABC_NUM * ABC_NUM * sizeof(float));
	const char* letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	for (int i = 0; i < ABC_NUM; i++)
	{
		for (int j = 0; j < ABC_NUM; j++)
			ABCgrid[i*ABC_NUM + j] = compareChars(letters[i],letters[j],weights);
	}
	
	return ABCgrid;
}

// ======================== n,k array according to mutant number

int* createNK(int num_mutants)
{
	int* nkArr = (int*) malloc(num_mutants * 2 * sizeof(int));

	int n = 0, k = 1;
	for (int i = 0; i < num_mutants; i++) 
	{
		nkArr[i] = n;
		nkArr[i+num_mutants] = k;

		n++;
		if (n == k)
		{
			n = 0;
			k++;
		} 
	}
	
	return nkArr;
}

// ========================

