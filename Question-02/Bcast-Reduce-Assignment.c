#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
 

int main(void)
 
{
  int my_rank, comm_sz;
  int i;
  int Count = 4;
  int Number[4];
  int Result[4];
	

  MPI_Init(NULL, NULL);

  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


  if(my_rank == 0)
    {
      printf("The input sequence is: \n");

      for (i = 0; i < Count; i++)
	{
	  Number[i] = i;
	  printf("%d ", Number[i]);
	}

      printf("\n");
    }

  // Process 0 sends data to all of the processes
  MPI_Bcast(Number, Count, MPI_INT, 0, MPI_COMM_WORLD);

  for (i = 0; i < Count; i++)
    Number[i] += my_rank;

  MPI_Reduce(Number       , Result, 4      , MPI_INT, MPI_PROD, 0, MPI_COMM_WORLD);

  // Print out the result 
  if (my_rank == 0)
    {
      printf("The output sequence is: \n");

		
      for (i = 0; i < Count; i ++)
	printf("%d ", Result[i]);

      printf("\n");
    }

  MPI_Finalize();

  return 0;
}

