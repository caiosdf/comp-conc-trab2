#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include <pthread.h>

typedef struct {
  int *array;
  int inicio;
  int fim;
} quickSort_parametros;

//função que verifica se o array está ordenado
int isSorted(int array[], int size)
{
  for (int i = 1; i < size; i++) {
    if (array[i] < array[i - 1]) {
      printf("na posição %d, %d < %d \n", i, array[i], array[i - 1]);
      return 0;
    }
  }
  printf("o array está ordenado corretamente\n");
  return 1;
}

void troca(int* a, int* b){
	int t = *a;
	*a = *b;
	*b = t;
}

//função de rearranjo do array utilizada no algoritmo do quicksort
int divide (int *array, int inicio, int fim, int pivot){
	int pivotVal = array[pivot];
    troca(&array[pivot],&array[fim]);
	int s = inicio; 

	for (int i = inicio; i <fim; i++){
		if (array[i] <= pivotVal){
			troca(&array[i], &array[s]);
            s++;
        } 
	}
	troca(&array[s], &array[fim]);
	return s;
}

void quickSort(int *array, int inicio, int fim){
	if (inicio < fim){
        int pivotPosition = inicio + (fim-inicio)/2;
        pivotPosition= divide(array, inicio, fim, pivotPosition);
		quickSort(array, inicio, pivotPosition - 1);
		quickSort(array, pivotPosition + 1, fim);
	}
}

void parallelQuickSort(int *array, int inicio, int fim);

void* parallel_quickSort_caller(void * params){
  quickSort_parametros* parametros = params;
  parallelQuickSort(parametros->array, parametros->inicio, parametros->fim);
  return NULL;
}

void parallelQuickSort(int *array, int inicio, int fim){

    if (inicio < fim){

        int pivotPos = inicio + (fim - inicio)/2;
        pivotPos = divide(array, inicio, fim, pivotPos);
        pthread_t thread;
        quickSort_parametros thread_param = {array, inicio, pivotPos-1};
		pthread_create(&thread, NULL, parallel_quickSort_caller, &thread_param);
		parallelQuickSort(array, pivotPos+1, fim);
        pthread_join(thread, NULL);
    }
}

int main(int argc, char *argv[]){

    double ini, fim; //variaveis utilizadas para medir intervalos de tempo
    int size; //variavel utilizada para guardar o tamanho do array

    //recebe e valida os paramentros de entrada
    if(argc < 2){
        fprintf(stderr, "Digite: %s <dimensão do vetor> \n", argv[0]);
        return 1;
    }

    size = atoll(argv[1]);

   int *array = malloc(size*  sizeof(long));

    //inicialização do vetor
    for (int i=0 ; i<size ; i++){
        array[i] = rand()%999;
    }
    /*
    printf("Vetor desordenado:\n");
	for(int i = 0; i < size; i++)
	{
		printf("%d ", array[i]);
	}
	printf("\n");
    */
    GET_TIME(ini);
	quickSort(array, 0, size - 1);
    GET_TIME(fim);
    /*
    printf("Vetor ordenado:\n");
	for(int i = 0; i < size; i++){
		printf("%d ", array[i]);
	}
    printf("\n");
    */
    printf("Tempo sequencial: %lf\n", fim-ini);
    
    
    GET_TIME(ini);
    parallelQuickSort(array, 0, size - 1);
    GET_TIME(fim);
    /*
    printf("Vetor ordenado:\n");
    for(int i = 0; i < size; i++){
		printf("%d ", array[i]);
	}
    printf("\n");
    */
    printf("Tempo paralelo: %lf\n", fim-ini);
    isSorted(array, size);
    free(array);
    
	return 0;
}