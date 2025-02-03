/*
Header file to process the input.*/

#ifndef PROCESS_INPUT_H
#define PROCESS_INPUT_H

// Include for size_t
#include <stdlib.h>

// Include for FILE*
#include <stdio.h>

// Include structs for INPUT_OUTPUT_MAPPING
#include "../structs/structs.h"

// Include config_manager for configurations
#include "../../configurations/config_manager.h"

// Function to parse the input csv into readable datastructures
INPUT_OUTPUT_MAPPING* parse_csv(const char*);

/*
Function to parse a CSV of doubles into a data structure that is readable by the Neural Network
ARGS: const char* filename
Returns the data as an INPUT_OUTPUT_MAPPING*
In case of an error, it returns NULL*/
void free_input_output_mapping(INPUT_OUTPUT_MAPPING*);

THREAD_DATA* distribute_data_among_threads(INPUT_OUTPUT_MAPPING*);

void free_thread_data_array(THREAD_DATA*);

/*
Function to initialise an INPUT_OUTPUT_MAPPING
Returns NULL if it fails*/
INPUT_OUTPUT_MAPPING* initialise_mapping(void);

/*
Function to read the next line of a csv file and store the values in an INPUT_OUTPUT_MAPPING
Returns SUCCESSFULL_EXECUTION_CODE if successfull and an error code if not.
Returns END_OF_CSV if the end of the CSV is reached*/
int read_line_csv(FILE*, INPUT_OUTPUT_MAPPING*);


/* 
Function to read the first line of the CSV and put its output into the mapping
Returns the number of doubles in the first line.
If there is a problem, it returns 0.*/
size_t read_first_line(FILE*, INPUT_OUTPUT_MAPPING*);


/*
|----------------------|
| Calculated CONSTANTS |
|----------------------|
*/

// Forward definition of the constants for the thread data functions
extern size_t BASE_SIZE_THREAD_DATA;
extern size_t REMAINDER_THREAD_DATA;
#endif // PROCESS_INPUT_H