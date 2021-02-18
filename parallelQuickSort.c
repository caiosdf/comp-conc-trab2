#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <sys/time.h>

double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */

typedef struct qsort_starter
{
  int *arr;
  int low;
  int high;
  int depthOfThreadCreation;
} quickSort_parameters;

void swap(int* a, int* b)
{
	int t = *a;
	*a = *b;
	*b = t;
}


int partition (int *arr, int low, int high, int pivot)
{
	int pivotValue = arr[pivot];
  swap(&arr[pivot],&arr[high]); // pivot
	int s = low; // Index of smaller element

	for (int i = low; i <high; i++)
	{
		// If current element is smaller than or
		// equal to pivot
		if (arr[i] <= pivotValue)
		{
			swap(&arr[i], &arr[s]);
      s++; // increment index of smaller element
		}
	}
	swap(&arr[s], &arr[high]);
	return s;
}


/*Sequencial QuickSort version*/
void quickSort(int *arr, int low, int high)
{
	if (low < high)
	{
    int pivotPosition = low+ (high-low)/2;
    pivotPosition= partition(arr, low, high, pivotPosition);
		quickSort(arr, low, pivotPosition - 1);
		quickSort(arr, pivotPosition + 1, high);
	}
}

void concurrent_quickSort(int *arr, int low, int high, int depthOfThreadCreation);




void* worker_quickSort(void * initialValues){
  quickSort_parameters* parameters = initialValues;
  concurrent_quickSort(parameters->arr, parameters->low, parameters->high,parameters->depthOfThreadCreation);
  return NULL;
}



void concurrent_quickSort(int *arr, int low, int high, int depthOfThreadCreation){
    if (low < high){

          int pivotPos = low + (high - low)/2;
          pivotPos = partition(arr, low, high, pivotPos);
          pthread_t thread;


        if (depthOfThreadCreation > 0){
            //printf("Am i here??");
            quickSort_parameters thread_param = {arr, low, pivotPos-1, depthOfThreadCreation};
            int result;
            //Creating the thread  by calling the pthread_create for the left part recursively
            result = pthread_create(&thread, NULL, worker_quickSort, &thread_param);

            //calling recursively for second time for the right part (for the same thread)
            concurrent_quickSort(arr, pivotPos+1, high, depthOfThreadCreation);


            pthread_join(thread, NULL);

        } else{
            //printf("\n");
            //printf("I am here\n");
            quickSort(arr, low, pivotPos-1);
            quickSort(arr, pivotPos+1, high);
         }

    }

}

int main(int argc, char **argv){

    int depthOfThreadCreation = 0;
    depthOfThreadCreation = atoll(argv[1]);

    int size;
    size = atoll(argv[2]);




    int *arrayElements = malloc(size*  sizeof(long));

    for (int i=0 ; i<size ; i++){
        arrayElements[i] = rand()%999;
    }

    printf("Unsorted\n");
    for(int i=0; i<size; i++){
      printf(" %d ", arrayElements[i]);
    }

    start_time = read_timer();
    concurrent_quickSort(arrayElements, 0, size-1, depthOfThreadCreation);
    end_time = read_timer();


    printf("\n");
    printf("Sorted\n");

    for(int i=0; i<size; i++){
      printf("%d ", arrayElements[i]);
    }

        printf("\n");
        printf("The execution time was:  %lf secs\n", end_time - start_time);

     free(arrayElements);
     return 0;
}