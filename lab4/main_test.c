/**
  * ECE254 Linux Dynamic Memory Management Lab
  * @file: main_test.c
  * @brief: The main file to write tests of memory allocation algorithms
  */ 

/* includes */
/* system provided header files. You may add more */
#include <stdio.h>

/* non-system provided header files. 
   Do not include more user-defined header files here
 */
#include "mem.h"


int main(int argc, char *argv[])
{

	void *p, *q, *r, *t;

	//Best fit test cases
	best_fit_memory_init(1024);	

	printf("Test #1\n");
	//Allocate/Dealloc pointer, and show it automatically performs coalescence when returned
	p = best_fit_alloc(200);
	printf("External Fragmentation: %d\n", best_fit_count_extfrag(900));
	best_fit_dealloc(p);
	printf("External Fragmentation: %d\n", best_fit_count_extfrag(900));

	printf("Test #2\n");
	//Segment the block into numerous pieces
	p = best_fit_alloc(200);
	q = best_fit_alloc(300);
	printf("External Fragmentation: %d\n", best_fit_count_extfrag(900));
	best_fit_dealloc(p);
	printf("External Fragmentation: %d\n", best_fit_count_extfrag(900));

	printf("Test #3\n");
	//If there is a block size of the requested memory block, do not create a new memory manager
	p = best_fit_alloc(200);
	printf("External Fragmentation: %d\n", best_fit_count_extfrag(1000));
	best_fit_dealloc(p);
	printf("External Fragmentation: %d\n", best_fit_count_extfrag(900));
	
	printf("Test #4\n");
	//Show coalescene of middle blocks
	p= best_fit_alloc(50);
	printf("External Fragmentation: %d\n", best_fit_count_extfrag(1000));
	best_fit_dealloc(q);
	printf("External Fragmentation: %d\n", best_fit_count_extfrag(1000));
	best_fit_dealloc(p);
	printf("External Fragmentation: %d\n", best_fit_count_extfrag(1000));


	printf("Test #5\n");
	//User can write data into the allocated pointer
	r = best_fit_alloc(30);
	char b[30] = "This is a test for best fit";
	memcpy(r, b, strlen(b));
	printf("Test for pointer allocation: %s\n", r);
	best_fit_dealloc(r);


	//Worst_fit test cases
	worst_fit_memory_init(1024);

	printf("Test #1\n");
	//Allocate/Dealloc pointer, and show it automatically performs coalescence when returned
	q = worst_fit_alloc(200);
	printf("External Fragmentation: %d\n", worst_fit_count_extfrag(900));
	worst_fit_dealloc(q);
	printf("External Fragmentation: %d\n", worst_fit_count_extfrag(900));

	printf("Test #2\n");
	//Segment the block into numerous pieces
	p = worst_fit_alloc(200);
	q = worst_fit_alloc(300);
	r = worst_fit_alloc(250);
	printf("External Fragmentation: %d\n", worst_fit_count_extfrag(900));
	worst_fit_dealloc(p);
	printf("External Fragmentation: %d\n", worst_fit_count_extfrag(900));

	printf("Test #3\n");
	//If available block size is same size of requested allocation, does not create a new memory manager
	p = worst_fit_alloc(200);
	printf("External Fragmentation: %d\n", worst_fit_count_extfrag(900));
	worst_fit_dealloc(r);
	worst_fit_dealloc(p);

	printf("Test #4\n");
	//Returned block is largest block, which is 476Bytes compared to 200bytes previously returned
	p= worst_fit_alloc(50);
	printf("External Fragmentation: %d\n", worst_fit_count_extfrag(1000));
	//Colescence when returned
	worst_fit_dealloc(q);
	printf("External Fragmentation: %d\n", worst_fit_count_extfrag(1000));
	worst_fit_dealloc(p);
	printf("External Fragmentation: %d\n", worst_fit_count_extfrag(1000));


	printf("Test #5\n");
	//User can write data into the allocated pointer
	p = worst_fit_alloc(40);
	char a[40] = "This is a test for worst fit";
	memcpy(p, a, strlen(a));
	printf("Test for pointer allocation: %s\n", p);
	worst_fit_dealloc(p);

	return 0;
}
