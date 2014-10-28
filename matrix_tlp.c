#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include <sys/resource.h> 
#include <pthread.h>

#define N 1000
int num_thread;

void usage_stats(struct rusage *pUsage)
{
	printf("N. page faults serviced without any I/O activity                %ld\n",  pUsage->ru_minflt);
	printf("N. page faults serviced that required any I/O                   %ld\n",  pUsage->ru_majflt);
	printf("N. context switch                     				%ld\n",  pUsage->ru_nivcsw);
}

int **A, **B, **C;

void *runner(void *param);

void populate(int** pMatrix)
{
	int row, column;
	srand(time(NULL));
	for (row = 0; row < N; row ++)
		for (column = 0; column < N; column++) {
			pMatrix[row][column] = rand() % 10 + 1;
		}
}

int main(int argc, char* argv[])
{

	struct rusage usage;
	pthread_t* thread;
	int row, column, aux;
 
	if (argv[1]==NULL){
		fprintf(stderr, "Missing parameter for number of threads\n");
		return -1;
	}

	num_thread = atoi (argv[1]);
	thread = (pthread_t*) malloc (num_thread*sizeof(pthread_t));

	printf("Matrix multiplication with %d thread\n", num_thread);

	A = (int**) malloc(N*sizeof(int*));
	B = (int**) malloc(N*sizeof(int*));
	C = (int**) malloc(N*sizeof(int*));
	for (int i = 0; i < N; i++)
	{
		A[i] = (int*) malloc(N*sizeof(int));
		B[i] = (int*) malloc(N*sizeof(int));
		C[i] = (int*) malloc(N*sizeof(int));
	}
	
	populate(A);
	populate(B);
	
	int i;
	for (i = 0; i < num_thread; i++)
		pthread_create(&thread[i], NULL, runner, (void*)i);
	
	for (i = 0; i < num_thread; i++)
		pthread_join (thread[i],NULL);

	free(thread);

	printf("\nComputation done!\n\n");

	getrusage(RUSAGE_SELF, &usage);

	usage_stats(&usage);

	return 0;			

}

void *runner(void* pSlice){
	int slice = (int)pSlice;
	int start = (slice * N)/num_thread;
	int end = ((slice+1) * N/num_thread);

	int row, column, k;

	int sum;
	for (row = start; row < end; row++)
		for (column = 0; column < N; column++)
		{
			sum = 0;
			for (k = 0; k < N; k++)
			{
				sum = sum + A[row][k]*B[k][column];
			}
			C[row][column] = sum;
		}

	pthread_exit(0);
}




