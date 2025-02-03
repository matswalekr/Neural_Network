/*
File of helper functions to work with the Neural Network.

Helper functions include printing the Network in different forms.

No function may alter or interfere with the structs or functions.
*/

#include <stdio.h>
#include <stdlib.h>

#include "helper_functions.h"


/*
Helper function to print the input output mapping*/
void print_mapping(INPUT_OUTPUT_MAPPING* mapping_to_print){

    size_t input_output_pairs_to_print = mapping_to_print->used;

    // Iterate over every input output pair
    for (size_t data_point = 0; data_point < input_output_pairs_to_print; data_point++){

        double* input_arr = mapping_to_print->input[data_point];
        double* output_arr = mapping_to_print->output[data_point];

        FAT_CHAR* input_fat_char = double_arr_to_str(input_arr, DIMENSIONS_DATA);
        if (!input_fat_char){
            // Case when the double -> str conversion failed
            return;
        }
        // Unpack the fat char
        size_t len_input_str = input_fat_char->length;
        char* input_str = input_fat_char->string;

        FAT_CHAR* output_fat_char = double_arr_to_str(output_arr, NUM_OUTPUT);
        if (!output_fat_char){
            // Case when the double -> str conversion failed
            free_fat_char(input_fat_char);
            return;
        }
        // Unpack the fat char
        size_t len_output_str = output_fat_char->length;
        char* output_str = output_fat_char->string;


        printf("\n****************************************************************\n");

        // Get enough "-" to print the most amount 
        char repeated_[] = "------------------------------------\
        --------------------------------------------------------\
        --------------------------------------------------------";

        printf("\n\
|--------%.*s-|      |---------%.*s-|\n\
| Input: %s | == > | Output: %s |\n\
|--------%.*s-|      |---------%.*s-|\n",
        (int) len_input_str ,repeated_, (int) len_output_str, repeated_, // First amount of "-"
        input_str, output_str,                               // Two strings to print
        (int) len_input_str ,repeated_, (int) len_output_str, repeated_ // Second amount of "-"
        );

        free_fat_char(input_fat_char);
        free_fat_char(output_fat_char);
    }
    return;
}


/*
Helper function to print the data attributable to each thread.
Input is an array of THREAD_DATA.*/
void print_data_threads(THREAD_DATA* thread_data_arr){

    for (size_t thread_num = 0; thread_num < NUM_THREADS; thread_num++){
        THREAD_DATA data = thread_data_arr[thread_num];

        // Print the number of the thread as header
        printf("\n\n\
*************************\n\
*    Thread: %02zu         *\n\
*************************\n", 
        (thread_num + 1));

        // Get enough "-" to print the most amount 
        char repeated_[] = "------------------------------------\
        --------------------------------------------------------\
        --------------------------------------------------------";

        size_t num_thread_data = data.size;

        for (size_t data_num = 0; data_num < num_thread_data; data_num++){
            
            FAT_CHAR* input_fat_char = double_arr_to_str(data.input[data_num], DIMENSIONS_DATA);
            if (!input_fat_char){
                // Case when the double -> str conversion failed
                printf("Failure to initialise input FAT_CHAR\n");
                return;
            }
            // Unpack the fat char
            size_t len_input_str = input_fat_char->length;
            char* input_str = input_fat_char->string;

            FAT_CHAR* output_fat_char = double_arr_to_str(data.output[data_num], NUM_OUTPUT);
            if (!output_fat_char){
                // Case when the double -> str conversion failed
                printf("Failure to initialise output FAT_CHAR\n");
                free_fat_char(input_fat_char);
                return;
            }
            // Unpack the fat char
            size_t len_output_str = output_fat_char->length;
            char* output_str = output_fat_char->string;
            printf("\n\
|--------%.*s-|      |---------%.*s-|\n\
| Input: %s | == > | Output: %s |\n\
|--------%.*s-|      |---------%.*s-|\n",
            (int) len_input_str ,repeated_, (int) len_output_str, repeated_, // First amount of "-"
            input_str, output_str,                               // Two strings to print
            (int) len_input_str ,repeated_, (int) len_output_str, repeated_ // Second amount of "-"
            );

            free_fat_char(input_fat_char);
            free_fat_char(output_fat_char);
        }
    }
    return;
}


// This function can print the entire Neural Netwrok defined
void print_network(neuron** neuron_array){

    // Pointer pointing to the current neuron in the network array
    size_t neuron_ptr = 0;

    for (size_t layer = 0; layer < NUMBER_LAYERS; layer++){
        printf("\n\n\
**********************************************************************************************\n\
*                                      Layer %3zu                                             *\n\
**********************************************************************************************\n\
        ", layer);

        for(size_t i = 0; i < NEURON_NUMBERS[layer]; i++){
            // Print the neuron
            print_neuron(neuron_array[neuron_ptr]);
            // Increment the neuron_ptr
            neuron_ptr++;
        }

        printf("\n**********************************************************************************************\n\n\n");
    }
}


void print_neuron(neuron* neuron_to_print){

    // get a fat char with the formated string for neuron_to_print -> input
    FAT_CHAR* fat_char_input = fat_char(neuron_to_print->input);
    if (!fat_char_input){
        printf("Memory allocation error for FAT_CHAR input_str.\n");
        // Case when input_str failed
        exit(OTHER_ERROR);
    }
    // Unpack the fat char
    char* string_input = fat_char_input->string;
    int length_fat_char_input = (int) fat_char_input->length;

    // get a fat char with the formated string for neuron_to_print -> der_biases
    FAT_CHAR* fat_char_der_biases = fat_char(neuron_to_print->der_biases);
    if (!fat_char_der_biases){
        printf("Memory allocation error for FAT_CHAR der_biases.\n");
        // Case when input_str failed
        free_fat_char(fat_char_input);
        exit(OTHER_ERROR);
    }
    // Unpack the fat char
    char* string_der_biases = fat_char_der_biases->string;
    int length_fat_char_der_biases = (int) fat_char_der_biases->length;


    // Get enough "-" to print the most amount 
    char repeated_[] = "------------------------------------\
    --------------------------------------------------------\
    --------------------------------------------------------";

    printf("\n\n\
|--------------------------------|----------------|----------|--------%.*s|-------------%.*s|\n\
| next_layer: %18p | bias: %8.2f | pos: %3zu | Input: %s| Der_biases: %s|\n\
|--------------------------------|----------------|----------|--------%.*s|-------------%.*s|\n\n",
    length_fat_char_input, repeated_,       // First array "padding" first line
    length_fat_char_der_biases, repeated_,  // Second array "padding" first line
    (void*) neuron_to_print->next_layer,neuron_to_print->bias, neuron_to_print->pos, string_input, string_der_biases,
    length_fat_char_input, repeated_,       // First array "padding" third line
    length_fat_char_der_biases, repeated_   // Second array "padding" third line
    );


    free_fat_char(fat_char_input);
    free_fat_char(fat_char_der_biases);

    return;
}


void print_linked_list_connection(node_linked_list_connection* linked_list_conn){

    
    while(linked_list_conn != NULL){


        // get a fat char with the formated string for linked_list_conn -> der_weight
        FAT_CHAR* fat_char_der_weight = fat_char(linked_list_conn->der_weight);
        if (!fat_char_der_weight){
            printf("Memory allocation error for FAT_CHAR der_weight.\n");
            // Case when input_str failed
            exit(OTHER_ERROR);
        }
        // Unpack the fat char
        char* string_der_weight = fat_char_der_weight->string;
        int length_fat_char_der_weight = (int) fat_char_der_weight->length;
        

            // Get enough "-" to print the most amount 
        char repeated_[] = "------------------------------------\
        --------------------------------------------------------\
        --------------------------------------------------------";
        
        printf("\n\
|-----------------|--------------------------|---------------|------------%.*s|\n\
| data (pos): %3zu | next: %18p | weight: %5.2f | Der_weight:%s|\n\
|-----------------|--------------------------|---------------|------------%.*s|\n",
        length_fat_char_der_weight, repeated_,       // First array "padding" first line
        linked_list_conn->data->pos, (void*) linked_list_conn->next, linked_list_conn->weight, string_der_weight,
        length_fat_char_der_weight, repeated_        // First array "padding" third line
        );

        free_fat_char(fat_char_der_weight);

        // Move to the next node
        linked_list_conn = linked_list_conn->next;
    }
    printf("\n");
    return;
}


/*
Searches for a neuron in a linked list. If the neuron is found, it returns 0, else it returns -1.
Args: neuron* neuron_to_find, node_linked_list_connection* list_to_search*/
int find_ptr_linked_list(neuron* neuron_to_find, node_linked_list_connection* list_to_search){
    while(list_to_search != NULL){
        if (list_to_search->data == neuron_to_find){
            return 0;
        } else{
            list_to_search = list_to_search->next;
        }
    }
    return -1;
}


/*
Function to convert an array of doubles into a string.
Returns NULL if it fails*/
FAT_CHAR* double_arr_to_str(double*double_arr, size_t num_doubles){

    /*
    Allocate memory fo a buffer that has place for
    num_doubles * (7 char for the number + 1 char for "|" + 2 chars for " ") + 2 (1 for extra "|" at the beginning and 1 for null terminator at the end)
    */
    size_t size_buffer = (num_doubles * 20) + 2;
    char* buffer = calloc(size_buffer, 1); // Size of char is always 1
    if (!buffer){
        // Case when the memory allocation failed
        perror("Calloc failed when converting array of doubles to string.\n");
        return NULL;
    }
    // Pointer to the current position in the buffer
    size_t ptr_buffer = 0;

    // Add the initial "|"
    buffer[ptr_buffer] = '|';
    ptr_buffer++;

    // Iterate over the number of doubles to format and insert their values
    for(size_t i = 0; i < num_doubles; i++){
        // Add leading space
        buffer[ptr_buffer] = ' ';
        ptr_buffer++;

        int written = snprintf(&buffer[ptr_buffer], size_buffer - ptr_buffer, "%3.2f", double_arr[i]);
        if (written < 0 || (size_t)written >= size_buffer - ptr_buffer) {
            perror("snprintf failed or buffer overflow.\n");
            free(buffer);
            return NULL;
        }

        // Move the buffer pointer past the written string
        ptr_buffer += written;

        // Add a trailing space and "|"
        buffer[ptr_buffer] = ' ';
        ptr_buffer++;
        buffer[ptr_buffer] = '|';
        ptr_buffer++;
    }
    // Null-terminate the buffer
    buffer[ptr_buffer] = '\0';

    FAT_CHAR* return_fat_char = (FAT_CHAR*) malloc(sizeof(FAT_CHAR));
    if (return_fat_char == NULL){
        // Case where memory allocation failed
        perror("Memory allocation error for fat char in the input_str function.\n");
        free(buffer);
        return NULL;
    }

    return_fat_char->string = buffer;
    return_fat_char->length = ptr_buffer;

    return return_fat_char;  
}


/*
Converts the an array of type double[NUM_THREADS] of a neuron into a nice string for printing.
Returns NULL if it fails*/
FAT_CHAR* fat_char(double data[NUM_THREADS]) {

    /* Allocate memory for a buffer that has place for
    NUM_THREADS * (8 chars for the number + 1 char for "|" + 2 chars for " ") + 1 (for null terminator at the end)
    */
    size_t size_buffer = (NUM_THREADS * 11) + 1;

    char* buffer = calloc(size_buffer, sizeof(char));
    if (buffer == NULL) {
        perror("calloc failed when converting input to string.\n");
        return NULL;
    }

    // Pointer to the current position in the buffer
    size_t ptr_buffer = 0;

    // Iterate over NUM_THREADS to format and insert input values
    for (size_t i = 0; i < NUM_THREADS; i++) {
        // Add a leading "|" and space
        buffer[ptr_buffer] = '|';
        ptr_buffer++;
        buffer[ptr_buffer] = ' ';
        ptr_buffer++;

        // Format the input value as a string with two decimal places
        int written = snprintf(&buffer[ptr_buffer], size_buffer - ptr_buffer, "%.2f", data[i]);
        if (written < 0 || (size_t)written >= size_buffer - ptr_buffer) {
            perror("snprintf failed or buffer overflow.\n");
            free(buffer);
            return NULL;
        }

        // Move the buffer pointer past the written string
        ptr_buffer += written;

        // Add an trailing space
        buffer[ptr_buffer] = ' ';
        ptr_buffer++;
    }

    // Null-terminate the buffer
    buffer[ptr_buffer] = '\0';

    FAT_CHAR* return_fat_char = (FAT_CHAR*) malloc(sizeof(FAT_CHAR));
    if (return_fat_char == NULL){
        // Case where memory allocation failed
        perror("Memory allocation error for fat char.\n");
        free(buffer);
        return NULL;
    }

    return_fat_char->string = buffer;
    return_fat_char->length = ptr_buffer;

    return return_fat_char;
}


/*
Frees the memory of the FAT_CHAR*/
void free_fat_char(FAT_CHAR* fat_char_free){
    free(fat_char_free->string);
    free(fat_char_free);
}
