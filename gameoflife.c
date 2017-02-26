// gameoflife.c
// Name: YOUR NAME HERE
// JHED: YOUR JHED HERE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mpi.h"

#define DEFAULT_ITERATIONS 10
#define GRID_WIDTH  256
#define DIM  16     // assume a square grid

int main ( int argc, char** argv ) {

	int global_grid[ 256 ] = 
	 {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	
	int (*twod_grid)[DIM+2][DIM+2] = malloc(sizeof(int) * (DIM+2) * (DIM+2));
	// MPI Standard variable
	int num_procs;
	int ID, j, i;
	int iters = 0;
	int num_iterations;
	int count = 0;
	int c, r, starting_point, ending_point;
	for (r = 0; r < (DIM+2); r++) {
	  for (c = 0; c < (DIM+2); c++) {
	    if (r <= 0 || r >= 17) {
	      (*twod_grid)[r][c] = 0;
	    } else {
	      if (c > 0 && c < 17){
		(*twod_grid)[r][c] = global_grid[count];
		count += 1;
	      }
	    }
	  }
	}
	// Setup number of iterations
	if (argc == 1) {
		num_iterations = DEFAULT_ITERATIONS;
	}
	else if (argc == 2) {
		num_iterations = atoi(argv[1]);
	}
	else {
		printf("Usage: ./gameoflife <num_iterations>\n");
		exit(1);
	}

	// Messaging variables
	MPI_Status stat;
	// TODO add other variables as necessary
	int num_percent, num_blocks, MPIDATA_TOUP, z, y;
	// MPI Setup
	if ( MPI_Init( &argc, &argv ) != MPI_SUCCESS)
	{
		printf ( "MPI_Init error\n" );
	}

	MPI_Comm_size ( MPI_COMM_WORLD, &num_procs ); // Set the num_procs
	MPI_Comm_rank ( MPI_COMM_WORLD, &ID );

	assert ( DIM % num_procs == 0 );
	num_percent = DIM / num_procs;
	num_blocks = DIM / num_percent;
	MPIDATA_TOUP = 1;
	// TODO Setup your environment as necessary

	for ( iters = 0; iters < 2; iters++ ) {
		// TODO: Add Code here or a function call to you MPI code
	  int up_ID = (ID - 1 + num_procs) % num_procs;
	  int down_ID = (ID + 1) % num_procs;
	  if (up_ID >= ID) {
	    MPI_Send(&((*twod_grid)[ID * num_blocks + 1][0]), DIM, MPI_INT, 
		     up_ID, MPIDATA_TOUP, MPI_COMM_WORLD);
	    MPI_Recv(&((*twod_grid)[down_ID * num_blocks + 1][0]), DIM, MPI_INT, 
		     down_ID, MPIDATA_TOUP, MPI_COMM_WORLD, &stat);
	  } else {
	    MPI_Recv(&((*twod_grid)[down_ID * num_blocks + 1][0]), DIM, 
		     MPI_INT, down_ID, MPIDATA_TOUP, MPI_COMM_WORLD, &stat);
	    MPI_Send(&((*twod_grid)[ID*num_blocks + 1][0]), DIM, MPI_INT, 
		     up_ID, MPIDATA_TOUP, MPI_COMM_WORLD);
	  }

	  if (down_ID >= ID) {
	    MPI_Send(&((*twod_grid)[(ID * num_blocks + num_blocks)][0]), DIM, MPI_INT, 
		     down_ID, MPIDATA_TOUP, MPI_COMM_WORLD);
	    MPI_Recv(&((*twod_grid)[(down_ID * num_blocks + num_blocks)][0]), DIM, 
		     MPI_INT, up_ID, MPIDATA_TOUP, MPI_COMM_WORLD, &stat);
	  } else {
	    MPI_Recv(&((*twod_grid)[(down_ID * num_blocks + num_blocks -1)][0]), DIM, 
		     MPI_INT, up_ID, MPIDATA_TOUP, MPI_COMM_WORLD, &stat);
	    MPI_Send(&((*twod_grid)[(ID * num_blocks + num_blocks - 1)][0]), DIM, MPI_INT, 
		     down_ID, MPIDATA_TOUP, MPI_COMM_WORLD);
	  }
	  //copy values
	  (*twod_grid)[0][0] = (*twod_grid)[DIM][DIM];
	  (*twod_grid)[0][DIM+1] = (*twod_grid)[DIM][0];
	  (*twod_grid)[DIM+1][0] = (*twod_grid)[0][DIM];
	  (*twod_grid)[DIM+1][DIM+1] = (*twod_grid)[1][1];
	  //copy rows and colums
	  for (i = 1; i < DIM; i++) {
	    (*twod_grid)[0][i] = (*twod_grid)[DIM][i];
	    (*twod_grid)[DIM+1][i] = (*twod_grid)[1][i];
	    (*twod_grid)[i][0] = (*twod_grid)[i][DIM];
	    (*twod_grid)[i][DIM+1] = (*twod_grid)[i][1];
	  }
	  // Life Operations in here
	  starting_point = ID * num_blocks;
	  ending_point = (ID * num_blocks + num_blocks - 1);
	  /*
	  for (i = starting_point; i <= ending_point; i++) {
	    for (j = 1; j <= DIM; j++) {
	      int num = 0;
	      int cur_value = (*twod_grid)[i][j];
	      for (z = i-1; z <= i + 1; z++) {
		for (y = j - 1; j <= j+1; j++) {
		  if (z != i && y != j && (*twod_grid)[z][y] == 1) {
		    num ++;
		  }
		}
	      }
	      if (cur_value == 0) {
		if (num == 3) {
		  (*twod_grid)[i][j] == 1;
		}
	      } else {
		if (num != 2 && num != 3) {
		  (*twod_grid)[i][j] = 0;
		}
	      }
	    }
	    }*/
		// Output the updated grid state
		if ( ID == 0 ) {
			printf ( "\nIteration %d: final grid:\n", iters );
			for (j = 0; j <= DIM+1; j++) {
			  for (i = 0; i <= DIM+1; i++) {
			    printf("%d  ", (*twod_grid)[j][i]);
			  }
			  printf("\n");
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	// TODO: Clean up memory
	MPI_Finalize(); // finalize so I can exit
}


