/*
 ============================================================================
 Name        : MPI_rebalance_application_cpp.c
 Author      : Yu
 Version     : 1.0
 Copyright   : Copy with reference to Yu
 Description : This application could re-balance the work of each process.
               It simulate how the process communicate with each other.
 ============================================================================
 */
#include <stdio.h>
#include <math.h> 
#include <mpi.h>

using namespace std;

/*
 * The main function to execute
 */
int main(int argc, char *argv[]) {

	// init the MPI
	MPI_Init(&argc, &argv);

	// get the size of the processes
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// get the rank
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	printf("%d %d", size, rank);

	MPI_Finalize();
	return 0;
}

