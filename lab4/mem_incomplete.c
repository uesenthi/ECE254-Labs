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
	size_t size;
	void* mem_address;
	struct node *next_free;
	//bool free;
};

struct node *first_block;
struct node *free_list;
struct node *busy_list;
/* Functions */

/* memory initializer */
int best_fit_memory_init(size_t size)
{
	/*
	// To be completed by students
	// You call malloc once here to obtain the memory to be managed.
	first_block = (struct node*) malloc(size);
	if(first_block == NULL){
		printf("Something went wrong with memory allocation\n");
		return -1;
	}
	else
	{
		first_block->size = size;
		first_block->mem_address = first_block + sizeof(struct node);//Available memory address
		first_block->next_free = NULL;
		//first_block->free = true;
		"Successully initialized memory"
		return 0;
	}
	*/
	first_block = (struct node*) malloc(size);
	if(first_block == NULL)
	{
		printf("Something went wrong with memory allocation");
		return -1;
	}
	else
	{
		free_list=first_block;
		free_list->size = size;
		//Allocate a large section for the two lists
		free_list->mem_address = free_list + 10*sizeof(struct node);//Available memory address
		first_block->next_free = NULL;
		//first_block->free = true;
		"Successully initialized memory"
		return 0;
	}

}

int worst_fit_memory_init(size_t size)
{

	return NULL;
}

/* memory allocators */
void *best_fit_alloc(size_t size)
{
	// To be completed by students
	// You call malloc once here to obtain the memory to be managed.
	//Cycle through every available memory block for the one with the smallest available size
	struct node *current_block = free_list;
	//printf("Current_block is %p\n",current_block);
	int diff = 1024;	//Initial largest possible difference
	struct node *chosen_block;
	struct node *remaining_block;
	
	do {
		if (current_block->size >= size && current_block->size - size <= diff) {
			diff = current_block->size - size;
			chosen_block = current_block;
		}
		current_block = current_block->next_free;
	}while(current_block != NULL);
	//Append chosen block to busy memory list

	if (chosen_block == NULL){
		printf("Could not choose a block");
		return NULL;
	}
	else
	{
		//Update block parameters for chosen block with allocated size
		busy_blocks = chosen_block;
		busy_blocks->size = size;
		busy_blocks->mem_address = busy_blocks + sizeof(struct node);
		printf("Chosen_block is %p\n",busy_blocks);

		//Compute remaining free space of block
		remaining_block = busy_blocks + busy_blocks->size;
		remaining_block->size = chosen_block->size - busy_blocks->size;
		remaining_block->mem_address = remaining_block + sizeof(struct node);
		printf("Remaining_block is %p\n",remaining_block);
		printf("%d\n",sizeof(struct node));
		
		//current_block->next_free = remaining_block;
		printf("Successful block allocation\n");
		return busy_blocks->mem_address;
	}


}


void *worst_fit_alloc(size_t size)
{
	// To be completed by students
	return NULL;
}

/* memory de-allocator */
void best_fit_dealloc(void *ptr) 
{
	struct node *return_block = ptr - sizeof(struct node);
	struct node *current_block = first_block;
	//Get the last free block
	while(current_block->next_free != NULL)
	{
		current_block = current_block->next_free;
	}
	//Append chosen block to busy memory list
	current_block->next_free = return_block;
	// To be completed by students
	return;
}

void worst_fit_dealloc(void *ptr) 
{

	// To be completed by students
	return;
}

/* memory algorithm metric utility function(s) */

/* count how many free blocks are less than the input size */ 
int best_fit_count_extfrag(size_t size)
{
	// To be completed by students
	return 0;
}

int worst_fit_count_extfrag(size_t size)
{
	// To be completed by students
	return 0;
}
