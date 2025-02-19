/*
Code to save the state of the neural network in a struct.*/

#include <stdio.h>
#include <stdlib.h>

#include "save_state.h"
#include "../../configurations/config_manager.h"
#include "../structs/structs.h"

/*
Function to allocate the memory for the STATE data structure.
Returns the structure when successfull.
Returns NULL if unsuccessfull*/
STATE* initialise_state(neuron** network_arr){

    // Allocate memory for the data structure
    STATE* state_ = malloc(sizeof(STATE));
    if (!state_){
        // Case where memory allocation error
        printf("Memory allocation error when initialising the STATE data structure.\n");
        return NULL;
    }

    // Allocate memory for the array of biases
    double* biases_arr = calloc(LENGTH_NETWORK, sizeof(double));
    if (!biases_arr){
        // Case where memory allocation error
        printf("Memory allocation error when initialising the biases_arr for the STATE.\n");
        free(state_); // Free the allocated memory
        return NULL;
    }


    // Allocate memory for the array of pointers to arrays of doubles representing the weights
    double** weight_ptr_arr = calloc(WORKING_NEURONS, sizeof(double*));
    if (!weight_ptr_arr){
        // Case where memory allocation error
        printf("Memory allocation error when initialisng the weight_ptr_arr for the STATE.\n");
        free(state_);     // Free the allocated memory
        free(biases_arr); // Free the allocated memory
        return NULL;
    }

    // Variable to store the number of connections for each neuron
    size_t count_connections;

    // Iterate over the Neural Network and allocate memory for the weights of each neuron
    for (size_t neuron_ptr = 0; neuron_ptr < WORKING_NEURONS; neuron_ptr++){
        count_connections = 0;

        // Get the connection to the next layer
        node_linked_list_connection* connection = network_arr[neuron_ptr]->next_layer;

        // Count the number of connections
        while (connection){
            count_connections++;
            connection = connection->next;
        }

        // Allocate memory for the number of connections
        double* connection_arr = calloc(count_connections, sizeof(double));
        if (!connection_arr){
            // Memory allocation error
            printf("Memory allocation error when initialisng the connection_arr of neuron %zu for the STATE.\n", neuron_ptr);
            free_state(state_); // Free the state (works because calloc initialises ptrs to NULL)
            return NULL;
        }
        
        // Assign the array_ptr
        weight_ptr_arr[neuron_ptr] = connection_arr;
    }

    // Assign the biases_arr
    state_->biases = biases_arr;

    // Assign the weight_arr
    state_->weights = weight_ptr_arr;

    return state_;
}


/*
Function to save the state of the neural network in a STATE data structure.
Saves the parameters inplace in the STATE.
*/
void save_state(neuron** network_arr, STATE* state){

    // Pointer to the current neuron in the array
    size_t neuron_ptr;

    // 1. Save the parameters of the WORKING_NEURONS
    for (neuron_ptr = 0; neuron_ptr < WORKING_NEURONS; neuron_ptr++){
        neuron* neuron_ = network_arr[neuron_ptr];

        // Save the bias
        state->biases[neuron_ptr] = neuron_->bias;

        // Save the weights
        double* weights_arr = state->weights[neuron_ptr];
        size_t arr_ptr = 0; // Pointer to the weights_arr
        // Get the connection to the next layer
        node_linked_list_connection* connection = neuron_->next_layer;
        while(connection){
            weights_arr[arr_ptr] = connection->weight;
            arr_ptr++;
            connection = connection->next;
        }
    }

    // 2. Save the parameters of the OUTPUT neurons
    for (;neuron_ptr < LENGTH_NETWORK; neuron_ptr++){
        neuron* output_neuron = network_arr[neuron_ptr];

        // Save the bias
        state->biases[neuron_ptr] = output_neuron->bias;

        // As the output neuron does not have connections, it also does not have weights
    }
}


/*
Function to restore the state of the neural network in a STATE data structure.
Updates the network_arr inplace.
*/
void restore_state(neuron** network_arr, STATE* state){

    // Pointer to the current neuron in the array
    size_t neuron_ptr;

    // 1. Restore the parameters of the WORKING_NEURONS
    for (neuron_ptr = 0; neuron_ptr < WORKING_NEURONS; neuron_ptr++){
        neuron* neuron_ = network_arr[neuron_ptr];

        // Restore the bias
        neuron_->bias = state->biases[neuron_ptr];

        // Restore the weights
        double* weights_arr = state->weights[neuron_ptr];
        size_t arr_ptr = 0; // Pointer to the weights_arr
        // Get the connection to the next layer
        node_linked_list_connection* connection = neuron_->next_layer;
        while(connection){
            connection->weight = weights_arr[arr_ptr];
            arr_ptr++;
            connection = connection->next;
        }
    }

    // 2. Save the parameters of the OUTPUT neurons
    for (;neuron_ptr < LENGTH_NETWORK; neuron_ptr++){
        neuron* output_neuron = network_arr[neuron_ptr];

        // Restore the bias
        output_neuron->bias = state->biases[neuron_ptr];

        // As the output neuron does not have connections, it also does not have weights
    }
}


/*
Function to free the STATE data structure*/
void free_state(STATE* state_to_free){
    // 1. Free the arrays of weights for each individual neuron
    for (size_t neuron_ptr = 0; neuron_ptr < WORKING_NEURONS; neuron_ptr++){
        free(state_to_free->weights[neuron_ptr]);
    }

    // 2. Free the biases and weights arrays
    free(state_to_free->weights);
    free(state_to_free->biases);

    // 3. Free the data structure itself
    free(state_to_free);

}

/*
Function to save the data of the neural */
int save_network_to_csv(neuron** network_arr, char* filename){
    FILE* csv_file = fopen(filename, "w");
    if (!csv_file){
        // Case when the file could not be opened
        printf("Could not open the file %s as csv.\n", filename);
        return FILE_ERROR;
    }

    /*********************************************************
     * 1. Write the basic architecture of the Neural Network *
     *********************************************************/
    // Write the first layer
    fprintf(csv_file, "%zu", NEURON_NUMBERS[0]);
    for (size_t i = 1; i < NUMBER_LAYERS; i++){
        // Write all other layers
        fprintf(csv_file, ",%zu", NEURON_NUMBERS[i]);
    }
    // New line
    fprintf(csv_file, "\n");

    /******************************************
     * 2. Write the specifics of every neuron *
     ******************************************/
    /*
    Note that the data is stored as follows:
    1. Bias of the Neuron
    2. Pos of the first connection
    3. Weight of the first connection
    4. Pos of the second connection
    5. Weigth of the third connection
    etc. 
    */

    for (size_t i = 0; i < LENGTH_NETWORK; i++){
        neuron* this_neuron = network_arr[i];

        // 1. Write the bias
        fprintf(csv_file,"%lf", this_neuron->bias);

        // 2. Write all the connections
        node_linked_list_connection* next_layer = this_neuron->next_layer;
        while (next_layer){
            // Write the position
            fprintf(csv_file,",%zu",next_layer->data->pos);
            // Write the weight
            fprintf(csv_file,",%lf",next_layer->weight);
            // Move to the next node
            next_layer = next_layer->next;
        }
        
        // 3. New Line
        fprintf(csv_file,"\n");
    }
    

    // Close the scv file
    fclose(csv_file);

    return SUCCESSFULL_EXECUTION_CODE;
}