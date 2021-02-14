/* Sequential Mandlebrot program */


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
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
	Window		win;                            /* initialization for a window */
	unsigned
	int             width, height,                  /* window size */
                        x, y,                           /* window position */
                        border_width,                   /*border width in pixels */
                        display_width, display_height,  /* size of screen */
                        screen;                         /* which screen */

	char            *window_name = "Mandelbrot Set", *display_name = NULL;
	GC              gc;
	unsigned
	long		valuemask = 0;
	XGCValues	values;
	Display		*display;
	XSizeHints	size_hints;
	Pixmap		bitmap;
	XPoint		points[800];
	FILE		*fp, *fopen ();
	char		str[100];
	
	XSetWindowAttributes attr[1];

       /* Mandlebrot variables */
        int i, j, k,rank,numprocs,l;
        Compl	z, c;
        float	lengthsq, temp;
        float* ponto;
        float** pontos;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);    
	/* connect to Xserver */
    if ( rank == 0 ) {
	if (  (display = XOpenDisplay (display_name)) == NULL ) {
	   fprintf (stderr, "drawon: cannot connect to X server %s\n",
				XDisplayName (display_name) );
	exit (-1);
	}
	
	/* get screen size */

	screen = DefaultScreen (display);
	display_width = DisplayWidth (display, screen);
	display_height = DisplayHeight (display, screen);

	/* set window size */

	width = X_RESN;
	height = Y_RESN;

	/* set window position */

	x = 0;
	y = 0;

        /* create opaque window */

	border_width = 4;
	win = XCreateSimpleWindow (display, RootWindow (display, screen),
				x, y, width, height, border_width, 
				BlackPixel (display, screen), WhitePixel (display, screen));

	size_hints.flags = USPosition|USSize;
	size_hints.x = x;
	size_hints.y = y;
	size_hints.width = width;
	size_hints.height = height;
	size_hints.min_width = 300;
	size_hints.min_height = 300;
	
	XSetNormalHints (display, win, &size_hints);
	XStoreName(display, win, window_name);

        /* create graphics context */

	gc = XCreateGC (display, win, valuemask, &values);

	XSetBackground (display, gc, WhitePixel (display, screen));
	XSetForeground (display, gc, DefaultColormap (display, screen));
	XSetLineAttributes (display, gc, 1, LineSolid, CapRound, JoinRound);

	attr[0].backing_store = Always;
	attr[0].backing_planes = 1;
	attr[0].backing_pixel = DefaultColormap(display, screen);

	XChangeWindowAttributes(display, win, CWBackingStore | CWBackingPlanes | CWBackingPixel, attr);

	XMapWindow (display, win);
	XSync(display, 0);
    }
        /* Calculate and draw points */
        ponto = malloc(sizeof(float)*2);
        pontos= malloc(sizeof(float)*2*Y_RESN*(X_RESN/numprocs));
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
        if (k == 10000 && rank == 0) {
        XDrawPoint (display, win, gc, j, i);
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
        for (j = 0; j < l; j++) {
        XDrawPoint (display, win, gc, pontos[j][1], pontos[j][0]);
        }
        }
        }

        else {
        MPI_Request req;
        MPI_Isend(&pontos, l, MPI_FLOAT, 0, 111, MPI_COMM_WORLD,&req);      
        }
	 
	XFlush (display);
	sleep (75);

	/* Program Finished */

}
