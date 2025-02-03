/*
File to test the helper functions. 

Includes neurons.h for seed_neurons to print the resulting network array.

This file may not be included in any other file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_helper_functions.h"
#include "../../neurons/neurons.h"

int test_print_neuron(void){
    neuron* this_neuron = (neuron*) malloc(size_neuron);

    if (this_neuron == NULL){
        // case where memory allocation failed
        return MEMORY_ALLOCATION_ERROR;
    }

    this_neuron->next_layer = NULL;
    this_neuron->bias = 22.34;
    this_neuron->pos = 2;

    // Initialise the input[NUM_THREADS]
    double values[NUM_THREADS] = {156752.34, 56.7878978, 90, 0};
    memcpy(this_neuron->input, values, sizeof(values));

    print_neuron(this_neuron);

    free(this_neuron);
    return SUCCESSFULL_EXECUTION_CODE;
}

int test_input_str(void){

    neuron neuron_ = {.input = {156752.34, 56.7878978, 90, 0}};


    FAT_CHAR* return_fat_char = input_str(&neuron_);

    if (return_fat_char == NULL){
        // Case where memory allocation failed
        return MEMORY_ALLOCATION_ERROR;
    }

    char* string = return_fat_char->string;

    printf("%s\n", string);

    printf("\nLength of fat char: %zu\n", return_fat_char->length);

    free_fat_char(return_fat_char);

    return SUCCESSFULL_EXECUTION_CODE;
}

int test_print_network(void){
    neuron** network_arr = seed_neural_network();

    if (network_arr == NULL){
        // Case where memory allocation fails
        perror("Memopry allocation error in test_print_network()");
        return MEMORY_ALLOCATION_ERROR;
    }

    // Print the network
    print_network(network_arr);

    // Free the network allocated in seed_network
    free_network(network_arr);

    return SUCCESSFULL_EXECUTION_CODE;
}

int test_print_linked_list_connection(void){
    node_linked_list_connection* node = (node_linked_list_connection*) malloc(size_linked_list_connection);
    if (node == NULL){
        // Case where memory allocation failed
        perror("Memory allocation issue in test_print_linked_list_connection.");
        return MEMORY_ALLOCATION_ERROR;
    }
    
    // Create one neuron to set it to the data of the linked_list_connection
    neuron* neuron_ = (neuron*) malloc(size_neuron);
    if (neuron_ == NULL){
        // Case where memory allocation failed
        free(node);
        perror("Memory allocation issue in test_print_linked_list_connection.");
        return MEMORY_ALLOCATION_ERROR;
    }
    // set the position as it is printed
    neuron_->pos = 4;

    // We need to initialise node->next to NULL to inhibit unexpected behavior
    node->next = NULL;
    node->data = neuron_;

    // Print the linked list
    print_linked_list_connection(node);

    // Free the memory allocated for the printed node
    free(node);
    return SUCCESSFULL_EXECUTION_CODE;
}

int main(void){
    if (test_print_linked_list_connection() == SUCCESSFULL_EXECUTION_CODE){
        return 0;
    }else{
        return OTHER_ERROR;
    }
    //if (test_print_network() == SUCCESSFULL_EXECUTION_CODE){
        //return 0;
    //}else{
        //return OTHER_ERROR;
    //}

    //if (test_print_neuron() == SUCCESSFULL_EXECUTION_CODE){
        //return 0;
    //} else {
        //return OTHER_ERROR;
    //}
    //test_input_str();
}