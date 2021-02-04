#include "mpi.h"
#include <iostream>
#include <iomanip>

using namespace std;

static const int64_t n = 1L<<28;  // 2^30

int main(int argc, char *argv[]) {
	
	int rank,proc;
	double sum = 0;
	double t;
	unsigned i;
	double pi = 0;
	MPI_Init(&argc, &argv);
	double inicio = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc);
    
    for (i = rank; i < n ; i+=proc) {
		t = (i+0.5)/n;
		sum += 4.0/(1.0+t*t);
	}
	sum/=n;
	
	MPI_Reduce(&sum, &pi, 1, MPI_DOUBLE, MPI_SUM, 0,MPI_COMM_WORLD);
	if (!rank) {
		cout << setprecision(18) <<  "Soma pi = " << pi << endl;
		cout << "Tempo de execução: " << MPI_Wtime() - inicio << endl;
	}
	MPI_Finalize();
	
	return 0;
}

/* 
#include <stdio.h>
#define N 1000000000

int main(int argc, char *argv[]) { 
double pi = 0.0f; 
long i;
    for (i = 0; i < N; i++) {
         double t = (double) ((i+0.5)/N);
         pi += 4.0/(1.0+t*t);
    }
    printf("pi = %f\n",pi/N);
    return(0);
}
*/
