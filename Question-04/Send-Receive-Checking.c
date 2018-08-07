////////////////////////////////////////////////////////////////
//
// This is a simple send/receive program in MPI
//
// Compile:  mpicc Send-Receive-Checking.c -o  Send-Receive
// 
// Run:      mpiexec -n  2  ./Send-Receive
//
///////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
 
int main(int argc, char* argv[])
{
  int my_rank, comm_sz;
   
  int token = 1;
 
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	
  if (comm_sz != 2)
    {
      if (my_rank == 0)

	fprintf(stderr, "USAGE: mpiexec -n 2 %s\n", argv[0]);

      MPI_Finalize();

      return 0;
    }

  if(my_rank == 0)
    {
      
      MPI_Send(&token, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
      printf("\nProcess %d sent token %d to process 1.\n", my_rank, token);
    }

  else 
    {
      MPI_Recv(&token, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("Process %d received token %d from process 0.\n\n", my_rank, token);
    }

  MPI_Finalize();

  return 0;
}
