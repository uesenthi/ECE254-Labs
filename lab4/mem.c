/**
 * @file memory.c
 * @brief: ECE254 Lab: memory allocation algorithm comparison template file
 * @author: 
 * @date: 2015-11-20
 */

/* includes */
#include <stdio.h> 
#include <stdlib.h> 
#include "mem.h"

/* defines */

/* global varaiables */
struct node
{
	int size;
	int free;
	struct node *next_free;

};

struct node *best_first_block;
size_t best_total_size;

struct node *worst_first_block;
size_t worst_total_size;

/* Functions */

/* memory initializer */
int best_fit_memory_init(size_t size)
{
	// To be completed by students
	// You call malloc once here to obtain the memory to be managed.
	best_first_block = (struct node*) malloc(size);
	if(best_first_block == NULL){
		printf("Something went wrong with best_init memory allocation\n");
		return -1;
	}
	else
	{
		best_total_size = size;
		best_first_block->size = size - sizeof(struct node);
		best_first_block->next_free = NULL;
		best_first_block->free = 1;
		return 0;
	}
	

}

int worst_fit_memory_init(size_t size)
{
	// To be completed by students
	// You call malloc once here to obtain the memory to be managed.
	worst_first_block = (struct node*) malloc(size);
	if(worst_first_block == NULL){
		printf("Something went wrong with best_init memory allocation\n");
		return -1;
	}
	else
	{

		worst_total_size = size;
		worst_first_block->size = size - sizeof(struct node);
		worst_first_block->next_free = NULL;
		worst_first_block->free = 1;
		return 0;
	}
}

/* memory allocators */
void *best_fit_alloc(size_t size)
{
	// To be completed by students
	struct node *current_block = best_first_block;
	
	int diff = best_total_size;	//Initial largest possible difference
	struct node *chosen_block;
	struct node *remaining_block;
	
	//Cycle through all free blocks and choose the smallest satisfiable block
	do {
		if (((float)current_block->size-sizeof(struct node) >= size || current_block->size == size) && current_block->size - size <= diff
		 && current_block->free == 1) {
			diff = current_block->size - size;
			chosen_block = current_block;
		}
		current_block = current_block->next_free;
	}while(current_block != NULL);

	if (chosen_block->size == size)
	{
		chosen_block->free = 0;
		return (chosen_block + 1);
	}
	else if (diff == best_total_size){
		printf("No available block size of %d\n", size);
		return NULL;
	}

	else 
	{
		//Determine remaining space. Size of remaining block will also factor in the size of the new node manager
		remaining_block = (struct node*) ((void*) (chosen_block + 1) + sizeof(char)*size);
		remaining_block->size = chosen_block->size - size - sizeof(struct node);
		remaining_block->free = 1;

		//If chosen block is pointing to another block, ensure remaining
		//block ends up pointing to the same place
		if(chosen_block->next_free != NULL){
			remaining_block->next_free = chosen_block->next_free;
		}
		//Set allocated block to in use
		chosen_block->size = size;
		chosen_block->free = 0;

		//Point chosen block to new remainder block
		chosen_block->next_free = remaining_block;
		
		//Return block of space
		return (chosen_block + 1);
	}


}


void *worst_fit_alloc(size_t size)
{
	// To be completed by students
	struct node *current_block = worst_first_block;
	
	int diff = 0;	//Initial largest possible difference
	struct node *chosen_block;
	struct node *remaining_block;
	
	//Cycle through all free blocks and choose the smallest satisfiable block
	do {
		//printf("Current block size is %d\n", current_block->size - size > diff);
		if (((float)current_block->size-sizeof(struct node) >= size || current_block->size == size) && current_block->size - size >= diff
		 && current_block->free == 1) {
		//if (current_block->size-sizeof(struct node) >= size && current_block->size - size > diff && current_block->free == 1) {
			diff = current_block->size - size;
			chosen_block = current_block;
		}
		current_block = current_block->next_free;
	}while(current_block != NULL);

	if (chosen_block->size == size)
	{
		chosen_block->free = 0;
		return (chosen_block + 1);
	}
	else if (diff == 0){
		printf("No available block size of %d\n", size);
		return NULL;
	}
	else 
	{
		//Determine remaining space. Size of remaining block will also factor in the size of the new node manager
		remaining_block = (struct node*) ((void*) (chosen_block + 1) + sizeof(char)*size);
		remaining_block->size = chosen_block->size - size - sizeof(struct node);
		remaining_block->free = 1;
		//remaining_block->mem_address = remaining_block + 1;

		//If chosen block is pointing to another block, ensure remaining
		//block ends up pointing to the same place
		if(chosen_block->next_free != NULL){
			remaining_block->next_free = chosen_block->next_free;
		}
		//Set allocated block to in use
		chosen_block->size = size;
		chosen_block->free = 0;

		//Point chosen block to new remainder block
		chosen_block->next_free = remaining_block;

		return (chosen_block + 1);
	}


}

/* memory de-allocator */
void best_fit_dealloc(void *ptr) 
{
	//Set the block of memory as free
	struct node *return_block = ptr - sizeof(struct node);
	return_block->free = 1;

	//Perform coalescence with any neighbouring free blocks
	struct node *current_block = best_first_block;
	struct node *next_block = current_block->next_free;
	
	do {

		if(current_block->free == 1 && current_block->next_free->free == 1){

			//Add the two block sizes, overwriting the memory manager of the next block
			current_block->size = current_block->size + current_block->next_free->size + sizeof(struct node);

			//Have the first block point to the memory manager of the second block, and repeat this iteration for the updated current block
			//Also need to update the pointer of the next block, so that it points to nothing
			next_block = current_block->next_free;
			current_block->next_free = current_block->next_free->next_free;
			next_block->next_free = NULL;
			
		}
		

		else{
			current_block = current_block->next_free;
		}

	}while(current_block->next_free != NULL);

	return;
}

void worst_fit_dealloc(void *ptr) 
{
	//Set the block of memory as free
	struct node *return_block = ptr - sizeof(struct node);
	return_block->free = 1;

	//Perform coalescence with any neighbouring free blocks
	struct node *current_block = worst_first_block;
	struct node *next_block = current_block->next_free;
	
	do {

		if(current_block->free == 1 && current_block->next_free->free == 1){

			//Add the two block sizes, overwriting the memory manager of the next block
			current_block->size = current_block->size + current_block->next_free->size + sizeof(struct node);

			//Have the first block point to the memory manager of the second block, and repeat this iteration for the updated current block
			//Also need to update the pointer of the next block, so that it points to nothing
			next_block = current_block->next_free;
			current_block->next_free = current_block->next_free->next_free;
			next_block->next_free = NULL;
			
		}

		else{
			current_block = current_block->next_free;
		}

	}while(current_block->next_free != NULL);

	return;
}

/* memory algorithm metric utility function(s) */

/* count how many free blocks are less than the input size */ 
int best_fit_count_extfrag(size_t size)
{
	// To be completed by students
	int count = 0;
	struct node *current_block = best_first_block;
	do{
		
		if(current_block->size < size && current_block->free == 1){
			count++;
		}
		
		current_block = current_block->next_free;
	}while(current_block != NULL);
	return count;
	
}

int worst_fit_count_extfrag(size_t size)
{
	// To be completed by students
	int count = 0;
	struct node *current_block = worst_first_block;
	do{
		
		if(current_block->size < size && current_block->free == 1){
			count++;
		}

		
		current_block = current_block->next_free;
	}while(current_block != NULL);
	return count;
	
}
