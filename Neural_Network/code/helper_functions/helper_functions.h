/*
Header file for the helper functions.

Includes structs.h for the definition of different structs
*/

#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#ifdef PROCESS_INPUT_H
// Include process input to have access to the specifics of the data
#include "../process_input/process_input.h"
#endif //PROCESS_INPUT_H

// Include structs to use neuron and linked_list_connections
#include "../structs/structs.h"

/*****************************************************************
 * FAT_CHAR datastructure: string + length of chars as metadata. *
 * Used for effective printing. May only be used in this file.   *
 *****************************************************************/
typedef struct FAT_CHAR{
    char* string;
    size_t length;
} FAT_CHAR;

/*******************************************************
 * 1. Functions to print different structs for testing *
 *******************************************************/
void print_mapping(INPUT_OUTPUT_MAPPING*);
void print_data_threads(THREAD_DATA*);
void print_network(neuron**);
void print_neuron(neuron*);
void print_linked_list_connection(node_linked_list_connection*);


/****************************************************
 * 2. Function to search for a ptr in a linked list *
 ****************************************************/
int find_ptr_linked_list(neuron* ,node_linked_list_connection*);


/*************************************************
 * 3. Functions to convert arrays into FAT_CHARs *
 *************************************************/
FAT_CHAR* double_arr_to_str(double*, size_t);
FAT_CHAR* fat_char(double[NUM_THREADS]);


/**********************************
 * 4. Function to free a FAT_CHAR *
 **********************************/
void free_fat_char(FAT_CHAR*);
#endif // HELPER_FUNCTIONS_H


/*************************
 * 5. Macro to time code *
 *************************/
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define TIME_THIS(expr) ({                                              \
    clock_t start = clock();                                            \
    __typeof__(expr) result = (expr);  /* Store return value */         \
    clock_t end = clock();                                              \
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;         \
    printf("Time taken: %f seconds\n", time_taken);                     \
    result;  /* Return the result */                                    \
})
