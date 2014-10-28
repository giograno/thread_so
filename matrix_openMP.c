#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include <sys/resource.h> 
#include <pthread.h>

#define N	1000

void usage_stats(struct rusage *pUsage)
{
	printf("N. page faults serviced without any I/O activity                %ld\n",  pUsage->ru_minflt);
	printf("N. page faults serviced that required any I/O                   %ld\n",  pUsage->ru_majflt);
	printf("N. context switch                     				%ld\n",  pUsage->ru_nivcsw);
}

void populate(int** pMatrix)
{
	int row, column;
	srand(time(NULL));
	for (row = 0; row < N; row ++)
		for (column = 0; column < N; column++) {
			pMatrix[row][column] = rand() % 10 + 1;
		}
}

int main(){
	int row, column, k, sum;

	struct rusage usage;

	// no needs for global variables
	int** A;
	int** B;
	int** C;

	A = (int**) malloc(N*sizeof(int*));
	B = (int**) malloc(N*sizeof(int*));
	C = (int**) malloc(N*sizeof(int*));
	int i;
	for (i = 0; i < N; i++)
	{
		A[i] = (int*) malloc(N*sizeof(int));
		B[i] = (int*) malloc(N*sizeof(int));
		C[i] = (int*) malloc(N*sizeof(int));
	}

	populate(A);
	populate(B);

	#pragma omp parallel shared(A,B,C) private (row, column, k, sum)
	{
		#pragma omp for schedule (static)
		for (row = 0; row < N; row++)
		for (column = 0; column < N; column++)
		{
			sum = 0;
			for (k = 0; k < N; k++)
			{
				sum = sum + A[row][k]*B[k][column];
			}
			C[row][column] = sum;
		}
	}

	printf("\nComputation done!\n\n");

	getrusage(RUSAGE_SELF, &usage);

	usage_stats(&usage);
	return 0;
}

