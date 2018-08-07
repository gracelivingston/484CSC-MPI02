///////////////////////////////////////////////////////////////////////////////////////
//
// This is an MPI program that computes the standard deviation for an array of elements.
//
// Note: The array is generated according to its size (the number of elements), which 
//       is specified by users from the command line. In the program, we assume its 
//       maximum number is 10.
//
// Compile:  mpicc Standard-Deviation.c -o  Stddev -lm
// 
// Run:      mpiexec -n  <p>  ./Stddev  <N>
//
//           -p: the number of processes
//           -N: the number of elements 
//
///////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>
#include <math.h> 
#include <mpi.h> 
   
int *Random_Num_Generator(int Num_Elements);

int Compute_Sum(int *array, int num_elements);

int main(int argc, char** argv) 
{ 
  int comm_sz, my_rank;
  int i, Product;
	
  double local_start, local_finish, local_elapsed, elapsed;

  //Seed the random number generator to get different results each time 
  srand(time(NULL)); 
 
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);   
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);


  //The error checking here should be performed to ensure that the number of arguments is 2. Otherwise, the usage is printed.
  if (argc != 2)
    {
      if (my_rank == 0)

	fprintf(stderr, "USAGE: mpiexec -n <num_of_processes> Stddev <num_elements>\n");

      MPI_Finalize();

      return 0;
    }

  int Num_Elements = atoi(argv[1]);
	
  //The error checking here is performed to ensure that the size of vector is an integer in the range between 1 and 10.
  if (Num_Elements < 1 || Num_Elements > 10)
    {
      if (my_rank == 0)

	fprintf(stderr, "Error: The size of the vector should be an integer in the range between 1 and 10!\n");

      MPI_Finalize();

      return 0;
    }
	
  // The error checking here is performed to ensure that the number of elements are evenly divisible by the number of the processes
  if (Num_Elements % comm_sz != 0)   
    {
      if (my_rank == 0)

	fprintf(stderr, "Error: The size of the vector should be evenly divisible by the number of Processes!\n");

      MPI_Finalize();

      return 0;
    }

  // Generate a random array of elements on process 0
  int *Random_Nums = NULL;

  if (my_rank == 0)
    {
      Random_Nums = Random_Num_Generator(Num_Elements);

      printf("\nThe random numbers generated: \n");

      for (i = 0; i < Num_Elements; i++)

	printf("%d ", Random_Nums[i]);

      printf("\n");
    }

  // Compute number of random intergers on each process 
  int Num_Per_Proc = Num_Elements / comm_sz;

  // Generate a buffer for holding a subset of the entire array 
  int *Sub_Random_Nums = (int *)malloc(sizeof(int) * Num_Per_Proc);

  // Scatter the random integer numbers from process 0 to all processes 
  MPI_Scatter(Random_Nums, Num_Per_Proc, MPI_INT, Sub_Random_Nums, Num_Per_Proc, MPI_INT, 0, MPI_COMM_WORLD);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //                                 Start point for code to be timed
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

  MPI_Barrier(MPI_COMM_WORLD);

  local_start = MPI_Wtime();
 
  // Compute the sum of the subset array on each process
  int Sub_Sum = Compute_Sum(Sub_Random_Nums, Num_Per_Proc);

  // Compute the sum of the entire array and distribute it to each process
  int Total_Sum;

  MPI_Allreduce(&Sub_Sum, &Total_Sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  // Compute the average of the entire array on each process
  float Average = (float) Total_Sum / Num_Elements;

  // Compute the partial sum of the squared differences from the average on each process
  float Sub_Square_diff = 0;

  for (i = 0; i < Num_Per_Proc; i++)
	
    Sub_Square_diff += (Sub_Random_Nums[i] - Average) * (Sub_Random_Nums[i] - Average);

  // Reduce the total sum of the squared differences to the process 0 (root process)
  float Total_Square_diff;

  MPI_Reduce(&Sub_Square_diff, &Total_Square_diff, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

  // Compute the standard deviation 
  if (my_rank == 0) 
    {
      // The standard deviation is the square root of the averaged total sum of squared differences
      float Standard_Deviation = sqrt(Total_Square_diff / Num_Elements);

      printf("Standard deviation = %f\n", Standard_Deviation);
    }

	
  local_finish = MPI_Wtime();
  local_elapsed = local_finish - local_start;

  MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  if (my_rank == 0)

    printf("Elapsed time =  %e seconds\n", elapsed);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //                                  End point for Code to be timed
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // Clean up 
  if (my_rank == 0) 
	
    free(Random_Nums);
	   
  free(Sub_Random_Nums);
    
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize(); 

  return 0;
} 


// Create an array of random integer numbers ranging from 1 to 10 
int *Random_Num_Generator(int Num_Elements)
{
  int *Rand_Nums = (int *)malloc(sizeof(int) * Num_Elements);
  int i;

  for (i = 0; i < Num_Elements; i++)
    Rand_Nums[i] = (rand() % 10) + 1;

  return Rand_Nums;
}

// Computes the sum of an array of numbers 
int Compute_Sum(int *array, int num_elements)
{
  int sum = 0;
  int i;
	
  for (i = 0; i < num_elements; i++) 
    sum += array[i];

  return sum;
}
