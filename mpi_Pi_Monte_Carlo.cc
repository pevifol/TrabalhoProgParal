#include "mpi.h"
#include <iostream>
#include <iomanip>

using namespace std;

static const int64_t n = 1e9;

void

int main(int argc, char *argv[]) {
	
	int rank,proc;
	double x,y;
	int sum = 0;
	unsigned i,hit = 0;
	
	MPI_Init(&argc, &argv);
	double inicio = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc);
    srand(time(NULL)+rank);
    for (i = 0; i < n/proc ; i++) {
		x = (double) rand()/RAND_MAX;
		y = (double) rand()/RAND_MAX;
		if (x*x+y*y < 1) 
			hit++;			
	}
	cout << "Hits locais" << hit << endl;
	MPI_Reduce(&hit, &sum, 1, MPI_INT, MPI_SUM, 0,MPI_COMM_WORLD);
	
	if (!rank) {
		cout << setprecision(18) <<  "Soma pi = " << 4.0*sum/n << endl;
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
