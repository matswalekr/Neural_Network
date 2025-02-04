/*
This file is responsible for the creation of the Neural Network architecture.
In addition, it also holds the data for the freeing of the allocated data

The network will be created using random weights and biases

Includes structs.h to use the neuron and node_linked_list_connection structs
Includes time.h for random seeding.
Is prohibited from using any other header files

Error should always print a message with the necessary information.
Malloc errors return a MEMORY_ALLOCATION_ERROR (200 Error Code) or NULL if pointers are returned.
Error messages should be printed when they arise, not when they are processed.
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "neurons.h"

#ifdef TEST_MODE
// For testing the weights are initialised to 1 to transmit the data without changes throughout the Network
#define WEIGHT_INITIALISER() 1
// For testing the bias is initialised to 0 to transmit the data more easily
#define BIAS_INITIALISER() 0

#else
#define WEIGHT_INITIALISER() random_number()
#define BIAS_INITIALISER() random_number()
#endif // TEST_MODE

/*
Function to allocate space for and create a new neuron
Returns NULL if it fails*/
extern inline neuron* create_new_neuron(size_t pos){
    neuron* new_neuron = (neuron*) malloc(size_neuron);
    if (!new_neuron){
        return NULL;
    }
    new_neuron->pos = pos;            // Assign the position
    new_neuron->next_layer = NULL;    // Initialise the next_layer to NULL for now

    double bias = BIAS_INITIALISER(); // Initialise the bias
    new_neuron->bias = bias;          // Set the bias to the initialised number

    // Set all input fields to the respective bias.
    for (int i = 0; i < NUM_THREADS; i++){
        new_neuron->input[i] = bias;
    }

    return new_neuron;
}


/*
Allocated memory for an array and seeds the Neurons into it.
No connection or manipulation done yet.

In case of failure, the data will be freed immediately

In case of an error, it return NULL.*/
neuron** seed_neural_network(void){


    // Initialise the length constants of the neural network.
    initialise_length_network();

    // 1. Allocate memory for the array of pointers to neurons (and initialise all pointer to NULL)
    // Array of pointers to neurons (Initialised to be all NULL)
    neuron** network_array = (neuron**) calloc(LENGTH_NETWORK, sizeof(neuron*));

    if (network_array == NULL){
        // Memory allocation error
        perror("Memory allocation error when trying to allocate memory for network_array.\n");
        return NULL;
    }

    // 2. Initialise the neurons in the array with the respective values for its fields
    for (size_t pos = 0; pos < LENGTH_NETWORK; pos++){

        neuron* new_neuron = create_new_neuron(pos);

        if (new_neuron == NULL){
            // Case when memory allocation fails
            perror("Memory allocation error when creating neurons.\n");
            free_network(network_array);
            return NULL;
        }

        network_array[pos] = new_neuron; // Assign the neuron to the network_array
    }

    return network_array;
}


/*
Connects the neurons of the Neural Network.
Uses an initialised network_array.

In its default implementation, all neurons of one layer are connected to the next layer

In case of an error, the memory of the neuron_array will be freed by this function.

Normal Return code: SUCCESSFULL_EXECUTION_CODE*/
int connect_neurons(neuron** network_array){

    // 1. Seed the random number generator
    seed_random();

    size_t ptr = 0; // Pointer to the current neuron in the array

    // 2. Loop through the input and hidden layers and connect the neurons
    for (size_t layer = 0; layer < NUMBER_LAYERS-1; layer++){ // Go over each layer
        size_t num_neurons_this_layer = NEURON_NUMBERS[layer];
        size_t num_neurons_next_layer = NEURON_NUMBERS[layer+1]; // Get the number of neurons in the next layer to connect to them
        
        size_t ptr_last_neuron_this_layer = ptr + num_neurons_this_layer;

        for (size_t i = 0; i < num_neurons_this_layer; i++){ // Loop over the neurons of this layer (i not used after)
            size_t rel_ptr_next_layer; // Relative pointer to the neurons in the next layer

            // As pointers are null indexed, but the numbers for layers are 1 indexed, no need to add 1
            for (rel_ptr_next_layer = 0; rel_ptr_next_layer < num_neurons_next_layer; rel_ptr_next_layer++){ // Loop over the number of neurons in the next layer that will be connected to the current neuron
                size_t next_layer_neuron_index = ptr_last_neuron_this_layer + rel_ptr_next_layer;

                if (add_connection(network_array, ptr, next_layer_neuron_index) == MEMORY_ALLOCATION_ERROR){
                    free_network(network_array);
                    //free(output);
                    return MEMORY_ALLOCATION_ERROR;
                }
            }
            ptr++; // Increment the pointer to point to the next neuron
        }
    }

    // 3. Initialise the output layer (last layer of neurons)
    // This layer will be connected to the output neuron, which will be read by the system
    size_t number_neurons_output_layer = NEURON_NUMBERS[NUMBER_LAYERS - 1];

    for (size_t ptr_neuron_last_layer = 0; ptr_neuron_last_layer < number_neurons_output_layer; ptr_neuron_last_layer++){
        neuron* neuron_output_layer = network_array[ptr];
        neuron_output_layer->next_layer = NULL; // There is no next layer
        ptr++;
    }
    return SUCCESSFULL_EXECUTION_CODE;
}


/*Function to connect two neurons in a network array inplace.
Takes ptr_start and ptr_end as args, which are the location of the neurons to connect in the network array

Memory allocation errors are not handled in this function*/
int add_connection(neuron** network_array, size_t ptr_start, size_t ptr_end){

    // Create a new node in the linked list of connections to link the new node
    node_linked_list_connection* new_node = (node_linked_list_connection*) malloc(size_linked_list_connection);

    if (new_node == NULL){
        // Case where memory allocation failed
        perror("Memory allocation error when connecting two nodes.\n");
        return MEMORY_ALLOCATION_ERROR; // Exit code
    }

    new_node->data = network_array[ptr_end];               // The data of the new node will be the neuron situated in the respective position in the network array
    new_node->next = network_array[ptr_start]->next_layer; // Connect the new_node to the existing linked list of the neuron
    new_node->weight = WEIGHT_INITIALISER();               // Initialise the weight. In testing, this is simplified to be 1.
    network_array[ptr_start]->next_layer = new_node;       // Connect the new linked list to the neuron it belongs to

    return SUCCESSFULL_EXECUTION_CODE;
}


/*Function to deleted the connection between two neurons inplace.
Takes ptr_start and ptr_end as args, which are the location of the neurons to connect in the network array*/
int del_connection(neuron** network_array, size_t ptr_start, size_t ptr_end){

    neuron* neuron_to_delete_from = network_array[ptr_start];
    neuron* neuron_to_delete = network_array[ptr_end];

    node_linked_list_connection* current = neuron_to_delete_from->next_layer; // Select the linked list of connected neurons

    // Special case when we delete the first node (we can not jump it as there is no previous neuron)
    if (current->data == neuron_to_delete){
        neuron_to_delete_from->next_layer = current->next; // Skip that node directly
        return SUCCESSFULL_EXECUTION_CODE;
    }

    // Normal case for all other nodes
    while (current != NULL){                           // Iterate over the entire linked list
        if(current->next->data == neuron_to_delete){   // Case when the next neuron is the connection to delete
            current->next = current->next->next;       // Skip the next node by assigning the node after it as next
            return SUCCESSFULL_EXECUTION_CODE;
        }
        current = current->next; // Move to the next node
    }

    // If the neuron to delete is not found
    return OTHER_ERROR; // Error code
}


/*
Function to set the bias for a node in the neural network.
This has the problem that the bias is also used as input in the respective field.*/
void set_bias(neuron** network_arr, size_t pos, double val){
    network_arr[pos]->bias = val;
    for (size_t i = 0; i < NUM_THREADS; i++){
        network_arr[pos]->input[i] = val;
    }
}


// Function to free the neural network. As neuron* are initialised to be NULL, this works for uninitialised network arrays.
void free_network(neuron** network_array){
    for (size_t pos = 0; pos < LENGTH_NETWORK; pos++){
        neuron* curr = network_array[pos];

        if (curr == NULL){
            // Last initialised neuron reached
            return;
        }
        free_linked_list(curr->next_layer);
        free(curr);
    }
}


// Function to free the linked list of the neurons
void free_linked_list(node_linked_list_connection* current){
    node_linked_list_connection* temp;

    while (current != NULL){
        temp = current->next; // Save the next node
        free(current);        // Free the current node
        current = temp;       // Move to the next node
    }
}


// Seed the random function using the current time
void seed_random(void){
    // Seed the random number generator only once with the current time
    srand(time(NULL));
}


// Returns a random number
float random_number(void){
    // Cast random number to float, and normalise.
    return (float)rand() / (float)RAND_MAX;
}

