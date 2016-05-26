#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100000

struct my_array {
    int *array;
    int len;
};

my_array merge(my_array args1,my_array args2)
{


	my_array result;


	result.len = args1.len + args2.len;
	result.array = (int *)malloc(sizeof(int)*result.len);

	//printf("merge : n = %d\n",result.len);

	int a=0,b=0,i=0;

	while(a < args1.len && b < args2.len)
	{

		if (args1.array[a] < args2.array[b] )
			result.array[i++]=args1.array[a++];
		else
			result.array[i++]=args2.array[b++];
	}

	while (a < args1.len ) result.array[i++] = args1.array[a++];
	while (b < args2.len ) result.array[i++] = args2.array[b++];

	return result;
}


void swap(int *xp, int *yp) {
  int temp = *xp;
  *xp = *yp;
  *yp = temp;
}

// A function to implement bubble sort
void bubbleSort(int arr[], int n) {
  int i, j;
  for (i = 0; i < n-1; i++)      
    for (j = 0; j < n-i-1; j++)
      if (arr[j] > arr[j+1])
		swap(&arr[j], &arr[j+1]);
}

int isSorted(int *a, int size) {
  int i;
  for (i = 0; i < size - 1; i++) {
    if (a[i] > a[i + 1]) {
      return 0;
    }
  }
  return 1;
}

// Function to print an array
void printArray(int arr[], int size)
{
  int i;
  for (i=0; i < size; i++)
    printf("%d ", arr[i]);
  printf("\n");
}


int main(int argc, char** argv) {
	int i, n;
	clock_t start, end;
	double elapsed_time, t1, t2;

	MPI_Init(NULL, NULL);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int* A;
	my_array result;

	if(rank==0)
	{	

	
		t1 = MPI_Wtime();
		A = (int *)malloc(sizeof(int)*N);
		if (A == NULL) {
			printf("Fail to malloc\n");
			exit(0);
		}
		for (i=N-1; i>=0; i--)
			A[N-1-i] = i;

		if (isSorted(A, N))
		  printf("Array is sorted\n");
		else
		  printf("Array is NOT sorted\n");

		for(i=1;i<world_size;i++)
		{
			MPI_Send(A+(i*N/world_size), N/world_size, MPI_INT, i, 11,
           MPI_COMM_WORLD);
		}

		bubbleSort(A, N/world_size);

		result.array = A;
	 	result.len = N/world_size;
		
	}
	else{

		int* B;
		B = (int *)malloc(sizeof(int)*N/world_size);

		
		MPI_Recv(B, N/world_size, MPI_INT, 0, 11, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);

		//printf("\nstart %d of %d\n",rank,world_size);
		bubbleSort(B, N/world_size);


		MPI_Send(B, N/world_size, MPI_INT, 0, 11, MPI_COMM_WORLD);

		//printArray(B, N/world_size);
		

		//printf("end (%d)\n ",rank);
	}

	

	// if (isSorted(A, N))
	//   printf("Array is sorted\n");
	// else
	//   printf("Array is NOT sorted\n");
	  
	// bubbleSort(A, N);
	// printArray(&A[N-10], 10);
	
	// if (isSorted(A, N))
	//   printf("Array is sorted\n");
	// else
	//   printf("Array is NOT sorted\n");
	 

	if(rank == 0)
	{

		for(i=1;i<world_size;i++)
		{
			my_array temp;

			temp.array = (int *)malloc(sizeof(int)*N/world_size);
			temp.len = N/world_size;

			MPI_Recv(temp.array, N/world_size, MPI_INT, i, 11, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);

			result = merge(temp,result);

		}

		//printArray(result.array,N);
		A = result.array;

		t2 = MPI_Wtime();
		
		printArray(&A[N-10], 10);
	
		if (isSorted(A, N))
		  printf("Array is sorted\n");
		else
		  printf("Array is NOT sorted\n");

		printf( "Elapsed time MPI_Wtime is %f\n", t2 - t1 ); 

	}

	MPI_Finalize();
	return 0;
}
