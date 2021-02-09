#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <iostream>

using namespace std;

static const int TAM = 1<<26;

void merge(int arr[], int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    int *L, *R;
    L = (int *) malloc(n1*sizeof(int));
    R = (int *) malloc(n2*sizeof(int));
    
 
    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    int i = 0;
    int j = 0;
    int k = l;
 
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
    free(L);
    free(R);
}

void mergeSort(int* vec, int l, int r) {
	if (l>=r) return;
	
	int m = l + (r-l)/2;
	
	mergeSort(vec,l,m);
	mergeSort(vec,m+1,r);
	merge(vec,l,m,r);
}

int main(int argc, char *argv[]) { /* mpi_Merge_Sort.cc  */
	int rank, procs;
	int *array, *subarray;
	auto ini = MPI_Wtime();
	
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);     // NUM PROCS PRECISA SER 2^K
    srand(rank);
	if (!rank) {
		array = (int *) malloc(TAM*sizeof(int));
		for (int i = 0 ; i < TAM ; i++) {
			array[i] = rand()%100;
		}
	}
	subarray = (int *) malloc(TAM*sizeof(int)/procs);
	
	MPI_Scatter(array, TAM/procs, MPI_INT, subarray, TAM/procs, MPI_INT, 0, MPI_COMM_WORLD);
	
	mergeSort(subarray,0,TAM/procs-1);
	
	MPI_Gather(subarray, TAM/procs, MPI_INT, array, TAM/procs, MPI_INT, 0, MPI_COMM_WORLD);
	
	
	
	if (!rank) {
		for (int i = procs ; i > 1 ; i/=2){  
			for (int j = 0 ; j < i ; j+=2) {
				merge(array,TAM/i*j,TAM/i*(j+1)-1,TAM/i*(j+2)-1);
			}
		}
		/*cout << endl << " Estado final do array: " << endl;
		for (unsigned i = 0 ; i < TAM ; i++) {
			cout << array[i] << " ";
		}
		*/
		cout << "Tempo demorado: " << MPI_Wtime()-ini << endl;
		free(array);
	}
	free(subarray);
	
	MPI_Finalize();
	return(0);
}
