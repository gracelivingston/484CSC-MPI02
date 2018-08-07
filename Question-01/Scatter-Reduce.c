#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>
#include <mpi.h> 
   
int *Random_Num_Generator(int Num_Elements);

int Compute_Product(int *array, int num_elements);

int main(int argc, char* argv[]) 
{ 
   
  int comm_sz, my_rank;
  int i, Result;
  int Num_Per_Proc = atoi(argv[1]); 

  // Seed the random number generator to get different results each time 
  srand(time(NULL)); 
 
  MPI_Init(&argc, &argv);  

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);   
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

 
  // Generate a random array of elements on process 0
  int *Random_Nums = NULL;

  if (my_rank == 0)
    {
      Random_Nums = Random_Num_Generator(Num_Per_Proc * comm_sz);

      printf("The random numbers generated: \n");

      for (i = 0; i < Num_Per_Proc * comm_sz; i++)

	printf("%d; ", Random_Nums[i]);

      printf("\n");
    }


  // Generate a buffer for holding a subset of the entire array 
  int *Sub_Random_Nums = (int *)malloc(sizeof(int) * Num_Per_Proc);
 
  // Scatter the random integer numbers from process 0 to all processes 
  MPI_Scatter(Random_Nums, Num_Per_Proc, MPI_INT, Sub_Random_Nums, Num_Per_Proc, MPI_INT, 0, MPI_COMM_WORLD);
 
  // Compute the product value of a subset array on each process
  int Sub_Product = Compute_Product(Sub_Random_Nums, Num_Per_Proc);

  // Reduce the total product value of all elements to process 0
  MPI_Reduce(&Sub_Product, &Result       , 1, MPI_INT,     MPI_PROD  , 0, MPI_COMM_WORLD);


  if (my_rank == 0)
	
    printf("Total product of all elements is %d\n", Result);

  // Clean up 
  if (my_rank == 0) 
	
    free(Random_Nums);
	   
  free(Sub_Random_Nums);
   
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

// Computes the product of an array of numbers 
int Compute_Product(int *array, int num_elements)
{
  int product = 1;
  int i;
	
  for (i = 0; i < num_elements; i++) 
    product *= array[i];
	 
  return product;
}
