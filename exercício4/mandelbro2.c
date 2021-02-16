/* Sequential Mandlebrot program */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <mpi.h>

#define		X_RESN	1280       /* x resolution */
#define		Y_RESN	900       /* y resolution */

typedef struct complextype
	{
        float real, imag;
	} Compl;


void main(int argc, char *argv[])
{
       /* Mandlebrot variables */
        int i, j, k,rank,numprocs,l;
        Compl	z, c;
        float	lengthsq, temp;
        float* ponto;
        float** pontos;
        MPI_Init(&argc, &argv);
        MPI_Comm_rank (MPI_COMM_WORLD, &rank);
        MPI_Comm_size (MPI_COMM_WORLD, &numprocs);    
	width = X_RESN;
	height = Y_RESN;
        /* Calculate and draw points */
        ponto = malloc(sizeof(float)*2);
        pontos= malloc(sizeof(float)*2*Y_RESN*(X_RESN/numprocs));
	t0 = MPI_Wtime()
        l = 0;       
        for(i = rank; i < X_RESN; i = i + numprocs) { //Calculamos faixas vertícais. 
        for(j=0; j < Y_RESN; j++) {

          z.real = z.imag = 0.0;
          c.real = ((float) j - 800.0)/450.0;               /* scale factors for 800 x 800 window */
	      c.imag = ((float) i - 375.0)/450.0;
          k = 0;

          do  {                                             /* iterate for pixel color */

            temp = z.real*z.real - z.imag*z.imag + c.real;
            z.imag = 2.0*z.real*z.imag + c.imag;
            z.real = temp;
            lengthsq = z.real*z.real+z.imag*z.imag;
            k++;

          } while (lengthsq < 4.5 && k < 10000);
        if (k == 10000 && rank != 0 ) {
        ponto[0] = i;
        ponto[1] = j;
        pontos[l] = ponto;
        l++;
        }
        }
        }
        if (rank == 0 ){
        for (i = 0; i < numprocs; i++) {
        MPI_Status stat;
        MPI_Probe(i,MPI_ANY_TAG,MPI_COMM_WORLD,&stat);
        MPI_Get_count(&stat,MPI_FLOAT,&l);
        pontos = malloc(sizeof(float)*l);
        MPI_Recv(&pontos, l, MPI_FLOAT, i, MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
        }

        else {
        MPI_Request req;
        MPI_Isend(&pontos, l, MPI_FLOAT, 0, 111, MPI_COMM_WORLD,&req);      
        }
	 
        t1 = MPI_Wtime();
	if (rank == 0) {
	printf("Tempo da rodada: %f",t1-t0);
	}
	/* Program Finished */

}
