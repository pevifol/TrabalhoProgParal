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


void main (int argc, char *argv[])
{

       /* Mandlebrot variables */
        int i, j, k;
        Compl	z, c;
        float	lengthsq, temp,t1,t0;
        MPI_Init(&argc, &argv);
        MPI_Comm_rank (MPI_COMM_WORLD, &rank);
        MPI_Comm_size (MPI_COMM_WORLD, &numprocs);  
	/* set window size */

	width = X_RESN;
	height = Y_RESN;

	/* set window position */

	x = 0;
	y = 0;
	
        /* Calculate and draw points */
        t0 = MPI_Wtime()
        for(i=0; i < X_RESN; i++) 
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

        if (k == 10000);
        }
        t1 = MPI_Wtime();
	if (rank == 0) {
	printf("Tempo da rodada: %f",t1-t0);
	}
	/* Program Finished */

}
