/*
This file is responsible for taking the raw inout of data (as csv) and parsing and processing it into readable data for the Neural Neutwork
This file should also include all necessary functions for freeing the datastructures created in this file.

This file is prohibited from using any other header files apart from config.h

Errors should always be printed when they occur with a message containing all relevant information.
Malloc Errors in functions should return a NULL pointer.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process_input.h"

// Maximum length of a line (not important, only used for creating the buffer)
#define MAX_LINE_LENGTH 1024

// Create a global buffer and calculate its size
char buffer[MAX_LINE_LENGTH];
size_t size_buffer = sizeof(buffer);

// Calculate the size of a double to avoid constant recalculation
size_t size_double = sizeof(double);

// Define the variables that will be assigned later
size_t DIMENSIONS_DATA;
size_t SIZE_TRAIN;
size_t BASE_SIZE_THREAD_DATA;
size_t REMAINDER_THREAD_DATA;


/* 
Function to read the first line of the CSV and put its output into the mapping
Returns the number of doubles in the first line.
If there is a problem, it returns 0.*/
size_t read_first_line(FILE* csv_file, INPUT_OUTPUT_MAPPING* mapping){

    // Try to get the first line
    if (!fgets(buffer, size_buffer, csv_file)){
        // Case when getting the first line fails
        perror("Error when tring to get the first line of the csv. Perhaps empty.\n");
        return 0;
    }

    // Intermediate variable to count the number of doubles in the first line
    size_t var_num_doubles = 0;


    // An array that will first hold all doubles and then the output doubles will be taken off again.
    double* input_arr = NULL; // Initialised to NULL for the behavior of realloc()

    // Tokenise the line and parse into doubles
    char *token = strtok(buffer, ","); // strtok replaces the "," with the "\0" string deliminator

    while(token){

        // Used by strtod to detect where the valid numeric parsing ends
        // If entire token is successfully parsed, this endptr will point to the null deliminator (end of each token by strtok)
        // If unsuccessfull parsing, it will point to the first invalid character
        char *endptr;

        var_num_doubles++; // Increment the number of doubles per line
        double value = strtod(token, &endptr); // strtod -> string to double

        if (*endptr != '\0' && *endptr != '\n' && *endptr != '\r') {
            fprintf(stderr, "Invalid double value: %s\n", token);
            free(input_arr);
            return 0; // Error code for this function
        }else{
            // Increase the memory
            // Use temp to free the memory safely in case realloc fails
            double* temp = realloc((void*) input_arr, var_num_doubles * size_double);
            if (!temp){
                // Case when memory reallocation failed
                perror("Memory reallocation in the first line of the csv failed.\n");
                free(input_arr);
                return 0; // Error code of this functions
            }
            // If it worked, reassign the input_arr pointer to the new location
            input_arr = temp;

            input_arr[var_num_doubles - 1] = value; // Assign the value to the correct location in the array
        }
        token = strtok(NULL, ","); // move to the next token
    }
    // No need to reassign memory for the output_arr, we can simply split the other array
    // Position of where to split
    size_t split_index = var_num_doubles - NUM_OUTPUT;

    // Pointer to the output doubles
    double* output_arr = input_arr + split_index;

    // As we have now read the first line, the size of the mapping is 1
    mapping->used = 1;
    mapping->input[0] = input_arr; // Assign the location of the input array to the input
    mapping->output[0] = output_arr; // Assign the location of the output array to the output


    // return the total number of doubles found
    return var_num_doubles;
}

/*
Function to initialise an INPUT_OUTPUT_MAPPING
Returns NULL if it fails*/
INPUT_OUTPUT_MAPPING* initialise_mapping(void) {

    // Default initialise_size
    size_t initialise_size = 8;

    // Allocate memory for the mapping structure
    INPUT_OUTPUT_MAPPING* mapping = malloc(sizeof(INPUT_OUTPUT_MAPPING));
    if (!mapping) {
        perror("Memory allocation failed for the mapping structure.\n");
        return NULL;
    }

    // Allocate memory for input and output arrays of pointers
    mapping->input = calloc(initialise_size, sizeof(double*));
    if (!mapping->input) {
        perror("Memory allocation failed for input pointers array.\n");
        free(mapping);
        return NULL;
    }

    mapping->output = calloc(initialise_size, sizeof(double*));
    if (!mapping->output) {
        perror("Memory allocation failed for output pointers array.\n");
        free(mapping->input);
        free(mapping);
        return NULL;
    }

    // Initialize size and used
    mapping->size = initialise_size;
    mapping->used = 0;

    return mapping;
}

/*
Function to read the next line of a csv file and store the values in an INPUT_OUTPUT_MAPPING
Returns SUCCESSFULL_EXECUTION_CODE if successfull and an error code if not.
Returns END_OF_CSV if the end of the CSV is reached*/
int read_line_csv(FILE* csv_file, INPUT_OUTPUT_MAPPING* mapping){
    
    // Read the next line into the buffer
    if (fgets(buffer, size_buffer, csv_file)== NULL){
        // case when the end of the file is reached
        return END_OF_CSV;
    }

    double* input_arr = calloc(DIMENSIONS_DATA, size_double);
    if (!input_arr){
        // Case where memory allocation failed
        perror("Memory allocation error when trying to read a line from the csv file.\n");
        return MEMORY_ALLOCATION_ERROR;
    }
    // The number of doubles we have already assigned to the input array
    size_t loc_in_input = 0;

    double* output_arr = calloc(NUM_OUTPUT, size_double);

    if (!output_arr){
        // Case where memory allocation failed
        perror("Memory allocation error when trying to read a line from the csv file.\n");
        free(input_arr);
        return MEMORY_ALLOCATION_ERROR;
    }
    // The number of doubles we have already assigned to the output array
    size_t loc_in_output = 0;


    // Tokenise the line and parse into doubles
    char *token = strtok(buffer, ","); // strtok replaces the "," with the "\0" string deliminator

    // Iterate until the end of the line and store the values into the input and output arrays
    while (token){

        // Used by strtod to detect where the valid numeric parsing ends
        // If entire token is successfully parsed, this endptr will point to the null deliminator (end of each token by strtok)
        // If unsuccessfull parsing, it will point to the first invalid character
        char *endptr;

        double value = strtod(token, &endptr); // strtod -> string to double

        if (*endptr != '\0' && *endptr != '\n' && *endptr != '\r') {
            fprintf(stderr, "Invalid double value: %s\n", token);
            free(input_arr); 
            free(output_arr);
            return OTHER_ERROR;

        } else {
            // Check if we are still working on the input
            if (loc_in_input < DIMENSIONS_DATA){
                input_arr[loc_in_input] = value;
                loc_in_input++;
            }else{
                // Check if we are still in bounds for the output array
                if (loc_in_output < NUM_OUTPUT){
                    output_arr[loc_in_output] = value;
                    loc_in_output++;
                } else{
                    // Case when the line is longer than it should be
                    printf("A line of the CSV is longer than it should be. Its length is: %zu\nNUM_OUTPUT: %zu, DIMENSIONS_DATA: %zu\n", loc_in_input, (size_t) NUM_OUTPUT,DIMENSIONS_DATA);
                    free(input_arr); 
                    free(output_arr);
                    return OTHER_ERROR;
                }
            }

            // Move to the next token in line
            // if strok is called with NULL, it continues with the last input
            token = strtok(NULL, ",");
        }
    }

    if (loc_in_output != NUM_OUTPUT){
        // Case when there were not enough datapoint¡s in the output
        perror("A line in the csv has less doubles than others");
        free(input_arr);
        free(output_arr);
        return OTHER_ERROR;
    }
    
    // Allocate the newly created arrays to the mapping
    if (mapping->used < mapping->size){
        // Easy case where there is still space for the input_output array
        mapping->input[mapping->used] = input_arr;
        mapping->output[mapping->used] = output_arr;
        mapping->used++;
    } else{
        // Reallocate the input array
        double** temp = (double**) realloc(mapping->input, 2 *  mapping->size * sizeof(double*));
        if (!temp){
            // Case where a memory allocation error occured
            perror("Memory allocation error when trying to increase the size of the mapping->input\n");
            free(input_arr);
            free(output_arr);
            return MEMORY_ALLOCATION_ERROR; // Note that the freeing of the mapping will be handled by the toplevel function
        } else{
            mapping->input = temp;
        }

        // Reallocate the output array
        temp = (double**) realloc(mapping->output, 2 *  mapping->size * sizeof(double*));
        if (!temp){
            // Case where a memory allocation error occured
            perror("Memory allocation error when trying to increase the size of the mapping->output\n");
            free(input_arr);
            free(output_arr);
            return MEMORY_ALLOCATION_ERROR; // Note that the freeing of the mapping will be handled by the toplevel function
        } else{
            mapping->output = temp;
        }
        // Update the size of the mapping
        mapping->size = 2 * mapping->size;
        // Insert the input and output
        mapping->input[mapping->used] = input_arr;
        mapping->output[mapping->used] = output_arr;
        mapping->used++;
    }

    return SUCCESSFULL_EXECUTION_CODE;
}

/*
Function to parse a CSV of doubles into a data structure that is readable by the Neural Network
ARGS: const char* filename
Returns the data as an INPUT_OUTPUT_MAPPING*
In case of an error, it returns NULL*/
INPUT_OUTPUT_MAPPING* parse_csv(const char* filename){

    // Open the csv file in read mode
    FILE* csv_file = fopen(filename, "r");
    if (csv_file == NULL){
        // Case when reading the file was unsuccessfull
        perror("An error occured when trying to read the csv file.\n");
        return NULL; // NULL;
    }

    INPUT_OUTPUT_MAPPING* mapping = initialise_mapping();
    if (!mapping){
        // Case when the memory allocation for mapping did not work
        fclose(csv_file);
        return NULL;
    }

    // Read in the first line
    const size_t total_len_row = read_first_line(csv_file, mapping);
    if (total_len_row == 0){
        // Case where the read_first_line function failed
        fclose(csv_file);
        free_input_output_mapping(mapping);
        return NULL;
    }

    /* 1.1 Dimensions of the input variables. This is data specific and should be changed depending on the dataset. 
    Even though this can not be declared const, it may never be changed!!!*/
    DIMENSIONS_DATA = total_len_row - NUM_OUTPUT;

    // Read all other lines
    int exit_code = read_line_csv(csv_file, mapping);
    while (exit_code == SUCCESSFULL_EXECUTION_CODE){
        exit_code = read_line_csv(csv_file, mapping);
    }

    // Case if there was an issue in the read_line_csv function
    if (exit_code != END_OF_CSV){
        perror("Error when trying to read additional lines of the csv file\n");
        free_input_output_mapping(mapping);
        fclose(csv_file);
        return NULL;
    }

    SIZE_TRAIN = mapping->used; // The number of datapoints is the number of datapoints in the mapping

    // Constants to distribute the data for the individual threads
    BASE_SIZE_THREAD_DATA = SIZE_TRAIN / NUM_THREADS; // Base size of the arrays passed into each thread
    REMAINDER_THREAD_DATA = SIZE_TRAIN % NUM_THREADS; // Remainder that still needs to be distributed among the threads

    // Close the file again
    fclose(csv_file);
    return mapping; 
}

/*
Function to distribute the data read in by the csv among the different threads.
Frees the INPUT_OUTPUT_MAPPING itself
Returns NULL if it fails*/
THREAD_DATA* distribute_data_among_threads(INPUT_OUTPUT_MAPPING* mapping){
    
    THREAD_DATA* thread_data_arr = (THREAD_DATA*) calloc(NUM_THREADS, size_thread_data);
    if(!thread_data_arr){
        // case where memory allocation failed
        perror("Memory allocation error when distributing data among threads\n");
        return NULL; // Error code
    }

    // Counter of the amount of data points used already
    size_t data_used_so_far = 0;

    for(size_t thread_num = 0; thread_num < NUM_THREADS; thread_num++){
        THREAD_DATA* data_for_this_thread = &thread_data_arr[thread_num];

        size_t size_of_thread_data = BASE_SIZE_THREAD_DATA;
        if(thread_num < REMAINDER_THREAD_DATA){
            size_of_thread_data++; // Increment the size by one more to accomodate for remainder
        }

        // The thread takes over its part of the mapping data
        data_for_this_thread->input = mapping->input + data_used_so_far;   // et a pointer to the array of inputs pointing to the start of data for this thread
        data_for_this_thread->output = mapping->output + data_used_so_far; // Get a pointer to the array of ouputs pointing to the start of data for this thread

        data_for_this_thread->size = size_of_thread_data;
        data_used_so_far += size_of_thread_data;
    }

    free(mapping); // Note that we are not freeing the input_arr or output_arr as they are now owned by the threads (each thread owns a part of it)
    return thread_data_arr;
}

void free_thread_data_array(THREAD_DATA* thread_data_array){
    // As the input/output pointers of the first thread_data still points to the start of the data array, we can free the entire array this way.
    free(thread_data_array[0].input);
    free(thread_data_array[0].output);

    // Now free the entire array
    free(thread_data_array);
    return;
}

/*Function to free the input output mapping again in case an error occurs.
Note that during normal running, the mapping is freed when the threads take over its data.*/
void free_input_output_mapping(INPUT_OUTPUT_MAPPING* map){
    // Free each array of inputs
    for (size_t i = 0; i < map->size; i++){
        free(map->input[i]);
        //free(map->output[i]);
    }
    free(map->input);
    free(map->output);
    // free the map
    free(map);
    return;
}
