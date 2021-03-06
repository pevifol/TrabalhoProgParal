/* ------------------------------------------------------------------------
 * FILE: least-squares-pt2pt.c
 *  
 * PROBLEM DEION:
 *  The method of least squares is a standard technique used to find
 *  the equation of a straight line from a set of data. Equation for a
 *  straight line is given by 
 *	 y = mx + b
 *  where m is the slope of the line and b is the y-intercept.
 *
 *  Given a set of n points {(x1,y1), x2,y2),...,xn,yn)}, let
 *      SUMx = x1 + x2 + ... + xn
 *      SUMy = y1 + y2 + ... + yn
 *      SUMxy = x1*y1 + x2*y2 + ... + xn*yn
 *      SUMxx = x1*x1 + x2*x2 + ... + xn*xn
 *
 *  The slope and y-intercept for the least-squares line can then be 
 *  calculated using the following equations:
 *        slope (m) = ( SUMx*SUMy - n*SUMxy ) / ( SUMx*SUMx - n*SUMxx ) 
 *  y-intercept (b) = ( SUMy - slope*SUMx ) / n
 *
 * PROGRAM DEION:
 *  o This program computes a linear model for a set of given data.
 *  o Data is read from a file; the first line is the number of data 
 *    points (n), followed by the coordinates of x and y.
 *  o Data points are divided amongst processes such that each process
 *    has naverage = n/numprocs points; remaining data points are
 *    added to the last process. 
 *  o Each process calculates the partial sums (mySUMx,mySUMy,mySUMxy,
 *    mySUMxx) independently, using its data subset. In the final step,
 *    the global sums (SUMx,SUMy,SUMxy,SUMxx) are calculated to find
 *    the least-squares line. 
 *  o For the purpose of this exercise, communication is done strictly 
 *    by using the MPI point-to-point operations, MPI_SEND and MPI_RECV. 
 *  
 * USAGE: Tested to run using 1,2,...,10 processes.
 * 
 * AUTHOR: Dora Abdullah (MPI version, 11/96)
 * LAST REVISED: RYL converted to C (12/11)
 * ---------------------------------------------------------------------- */
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "mpi.h"

int main(int argc, char **argv) {

  double *x, *y;
  double mySUMx, mySUMy, mySUMxy, mySUMxx, SUMx, SUMy, SUMxy,
         SUMxx, SUMres, res, slope, y_intercept, y_estimate;
  double t1,t0;
  int i,j,n,myid,numprocs,naverage,nremain,mypoints,ishift;
  /*int new_sleep (int seconds);*/
  MPI_Status istatus;
  FILE *infile;

  infile = fopen("xydata", "r");
  if (infile == NULL) printf("error opening file\n");

  MPI_Init(&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myid);
  MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
  t0 = MPI_Wtime();
  /* ----------------------------------------------------------
   * Step 1: Process 0 reads data and sends the value of n
   * ---------------------------------------------------------- */
  if (myid == 0) {
    printf ("Number of processes used: %d\n", numprocs);
    printf ("-------------------------------------\n");
    printf ("The x coordinates on worker processes:\n");
    /* this call is used to achieve a consistent output format */
    /* new_sleep (3);*/
    fscanf (infile, "%d", &n);
    x = (double *) malloc (n*sizeof(double));
    y = (double *) malloc (n*sizeof(double));
    for (i=0; i<n; i++) {
      fscanf (infile, "%lf %lf", &x[i], &y[i]);
    }
    MPI_Bcast (&n, 1, MPI_INT, myid,MPI_COMM_WORLD);
  }
  else {
   MPI_Bcast (&n, 1, MPI_INT, myid,MPI_COMM_WORLD);
  }
  /* ---------------------------------------------------------- */
  
  naverage = n/numprocs;
  nremain = n % numprocs;

  /* ----------------------------------------------------------
   * Step 2: Process 0 sends subsets of x and y 
!  * ---------------------------------------------------------- */
   if (myid == 0) {
   for (i = 0;  i < numprocs; i++) {
   MPI_Bsend (&x, n, MPI_FLOAT, i,myid, MPI_COMM_WORLD);
   MPI_Bsend (&y, n, MPI_FLOAT, i,myid, MPI_COMM_WORLD);
   }
   }
   else {
   x = (double *) malloc (n*sizeof(double));
   y = (double *) malloc (n*sizeof(double));
   MPI_Recv (x, n, MPI_FLOAT, MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
   MPI_Recv (y, n, MPI_FLOAT, MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);   
   }
  /* ----------------------------------------------------------
   * Step 3: Each process calculates its partial sum
   * ---------------------------------------------------------- */
  mySUMx = 0; mySUMy = 0; mySUMxy = 0; mySUMxx = 0;
  if (myid == 0) {
    ishift = 0;
    mypoints = naverage;
  }
  for (j=0; j<mypoints; j++) {
    mySUMx = mySUMx + x[ishift+j];
    mySUMy = mySUMy + y[ishift+j];
    mySUMxy = mySUMxy + x[ishift+j]*y[ishift+j];
    mySUMxx = mySUMxx + x[ishift+j]*x[ishift+j];
  }
  
  /* ----------------------------------------------------------
   * Step 4: Process 0 receives partial sums from the others 
   * ---------------------------------------------------------- */
  if (myid != 0) {
    MPI_Send (&mySUMx, 1, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD);
    MPI_Send (&mySUMy, 1, MPI_DOUBLE, 0, 6, MPI_COMM_WORLD);
    MPI_Send (&mySUMxy,1, MPI_DOUBLE, 0, 7, MPI_COMM_WORLD);
    MPI_Send (&mySUMxx,1, MPI_DOUBLE, 0, 8, MPI_COMM_WORLD);
	    }
  else {
    SUMx = mySUMx; SUMy = mySUMy;
    SUMxy = mySUMxy; SUMxx = mySUMxx;
    for (i=1; i<numprocs; i++) {
      MPI_Recv (&mySUMx, 1, MPI_DOUBLE, i, 5, MPI_COMM_WORLD, &istatus);
      MPI_Recv (&mySUMy, 1, MPI_DOUBLE, i, 6, MPI_COMM_WORLD, &istatus);
      MPI_Recv (&mySUMxy,1, MPI_DOUBLE, i, 7, MPI_COMM_WORLD, &istatus);
      MPI_Recv (&mySUMxx,1, MPI_DOUBLE, i, 8, MPI_COMM_WORLD, &istatus);
      SUMx = SUMx + mySUMx;
      SUMy = SUMy + mySUMy;
      SUMxy = SUMxy + mySUMxy;
      SUMxx = SUMxx + mySUMxx;
    }
  }

  /* ----------------------------------------------------------
   * Step 5: Process 0 does the final steps
   * ---------------------------------------------------------- */
  if (myid == 0) {
    slope = ( SUMx*SUMy - n*SUMxy ) / ( SUMx*SUMx - n*SUMxx );
    y_intercept = ( SUMy - slope*SUMx ) / n;
    /* this call is used to achieve a consistent output format */
    /*new_sleep (3);*/
    printf ("\n");
    printf ("The linear equation that best fits the given data:\n");
    printf ("       y = %6.2lfx + %6.2lf\n", slope, y_intercept);
    printf ("--------------------------------------------------\n");
    printf ("   Original (x,y)     Estimated y     Residual\n");
    printf ("--------------------------------------------------\n");
    
    SUMres = 0;
    for (i=0; i<n; i++) {
      y_estimate = slope*x[i] + y_intercept;
      res = y[i] - y_estimate;
      SUMres = SUMres + res*res;
      printf ("   (%6.2lf %6.2lf)      %6.2lf       %6.2lf\n", 
	      x[i], y[i], y_estimate, res);
    }
    printf("--------------------------------------------------\n");
    printf("Residual sum = %6.2lf\n", SUMres);
    t1 = MPI_WTime();
    printf("Tempo de execução: %f",t1-t0);
  }

  /* ----------------------------------------------------------	*/
  MPI_Finalize();
}
