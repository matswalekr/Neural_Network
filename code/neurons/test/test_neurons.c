/*
File to test the neurons.c file
No actual code may be written in this file (to the extent that it changes the behavior of any other object/file)

Includes the functions from neurons.h helper_functions.h and its own header file

Includes helper_functions to use the function find_ptr_linked_list
No other external functions may be included*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "test_neurons.h"
#include "../neurons.h"
#include "../../helper_functions/helper_functions.h"


/* 
Function to create (malloc) and seed the network array and then test if the seeding is successfully.
Checks if the neurons have been properly initialised in the array.
*/
neuron** test_seeding(void){

    // Test that the function "works"
    neuron** dummy_network_array = seed_neural_network();
    if (dummy_network_array == NULL){
        return NULL;
    }
    // Test if the output is as expected
    for (size_t position = 0; position < LENGTH_NETWORK; position++){
        neuron* this_neuron = dummy_network_array[position];

        // Test that the neuron is not NULL (has been initialised)
        assert (this_neuron != NULL);

        // Test that the positions are initialised in the right way
        assert(this_neuron->pos == position);

        // Test that there are not yet any connections
        assert(this_neuron->next_layer == NULL);
    }
    print_network(dummy_network_array);

    return dummy_network_array;
}

/*
Function to test if the connect_neurons_function leads to the expected output*/
int test_connect_neurons(neuron**dummy_network_array){

    //Connect all neurons in the array
    assert(connect_neurons(dummy_network_array) == SUCCESSFULL_EXECUTION_CODE);//, dummy_output) == SUCCESSFULL_EXECUTION_CODE);


    // 1. Split the individual neurons of the dummy_network_array in seperate arrays based on layers
    size_t ptr_neuron = 0; // Points to the current neuron

    neuron** different_layers[NUMBER_LAYERS];// + 1]; // Array of pointers to arrays of neurons (+1 for output neuron)
    for (size_t layer = 0; layer < NUMBER_LAYERS; layer++){
        size_t number_neurons_in_this_layer = NEURON_NUMBERS[layer];

        // Create an array of the size of neurons in this layer and initialise them to NULL
        neuron** array_neurons_this_layer = (neuron**) calloc(number_neurons_in_this_layer, sizeof(neuron*));
        if (array_neurons_this_layer == NULL){
            // Case where memory allocation failed
            printf("Memory allocation for array of neurons in testing failed.\n No actual testing done.\n");
            return MEMORY_ALLOCATION_ERROR;
            // (Freeing the memory is handled by toplevel function)
        }

        // Assign the pointer to the array to the different layer array
        different_layers[layer] = array_neurons_this_layer;

        // Fill the array with the neurons of this layer
        size_t incr; // Relative pointer in the subarray of neurons of this layer
        for (incr = 0; incr < number_neurons_in_this_layer; incr++){
            array_neurons_this_layer[incr] = dummy_network_array[ptr_neuron];
            ptr_neuron++;
        }

        // Make sure the entire array has been filled
        assert(array_neurons_this_layer[number_neurons_in_this_layer-1] != NULL);
    }

    assert(ptr_neuron == LENGTH_NETWORK); // Check if we have used all neurons
    // (Note that this works due to a 1 difference in indexing)

    // Loop over the input and hidden layers to see if all layers are connected in the right way
    for(size_t layer = 0; layer < NUMBER_LAYERS-1; layer++){
        
        // The neurons of the layer currently examined
        neuron** layer_arr = different_layers[layer];
        // The number of neurons in the layer currently examined
        size_t number_neurons_this_layer = NEURON_NUMBERS[layer];

        // The neurons of the next layer
        neuron** next_layer_arr = different_layers[layer+1];
        // The number of neurons in the next layer
        size_t number_neurons_next_layer;
        if (layer < (NUMBER_LAYERS-1) ){
            number_neurons_next_layer = NEURON_NUMBERS[layer+1];

        } else{
            number_neurons_next_layer = 1;
        }

        // Loop over all neurons of this layer (use a relative pointer for the array of neurons of this layer)
        for (size_t neuron_ptr = 0; neuron_ptr < number_neurons_this_layer; neuron_ptr++){
            
            // Neuron that we will check
            neuron* this_neuron = layer_arr[neuron_ptr];

            // The connections that have been done (expected to be the entire next layer)
            node_linked_list_connection* actual_connections = this_neuron->next_layer;

            // Check if all connections have been made in the right way
            assert (match_neurons_linked_list_vs_array(actual_connections, next_layer_arr, number_neurons_next_layer) == SUCCESSFULL_EXECUTION_CODE);
        }
    }

    // Free the allocated memory
    for (size_t layer = 0; layer < NUMBER_LAYERS; layer++){
        free(different_layers[layer]);
    }
    return SUCCESSFULL_EXECUTION_CODE;
}

/*
Tests that neurons can be disconnected and connected again.
It takes an established connection, disconnects it, checks if it worked and then reconnects it.
Used to test the add_connection and del_connection functions*/
void test_disconnect_connect_neurons(neuron** dummy_neural_network){

    // First neuron (will be used for testing for practical purposes)
    neuron* first_neuron = dummy_neural_network[0];

    // A neuron of the next layer (the first connection in this case)
    neuron* neuron_in_next_layer = first_neuron->next_layer->data;

    // Find the position of the other neuron in the array (needed to delete it)
    size_t pos_neuron_next_layer = 0;
    while (dummy_neural_network[pos_neuron_next_layer] != neuron_in_next_layer){
        pos_neuron_next_layer++;
    }
    // Check that the neuron is currently in the linked list (not necessary, we selected it from there)
    assert(find_ptr_linked_list(neuron_in_next_layer, first_neuron->next_layer) == 0);

    // delete the connection
    del_connection(dummy_neural_network, 0, pos_neuron_next_layer);

    // Check that the neuron is now no longer in the connection linked list
    assert(find_ptr_linked_list(neuron_in_next_layer, first_neuron->next_layer) == -1);

    // add the connection again
    add_connection(dummy_neural_network, 0, pos_neuron_next_layer);

    // Check that the neuron is again in the linked list
    assert(find_ptr_linked_list(neuron_in_next_layer, first_neuron->next_layer) == 0);

    /* -------------------------------------
    Select another neuron if possible*/

    // This can only be checked if the second layer has at least 3 neurons
    if (NEURON_NUMBERS[1] >= 3){
        // Another neuron of the next layer (the third connection in this case)
        neuron_in_next_layer = first_neuron->next_layer->next->next->data;

        // Find the position of the other neuron in the array (needed to delete it)
        pos_neuron_next_layer = 0;
        while (dummy_neural_network[pos_neuron_next_layer] != neuron_in_next_layer){
            pos_neuron_next_layer++;
        }
        // Check that the neuron is currently in the linked list (not necessary, we selected it from there)
        assert(find_ptr_linked_list(neuron_in_next_layer, first_neuron->next_layer) == 0);

        // delete the connection
        del_connection(dummy_neural_network, 0, pos_neuron_next_layer);

        // Check that the neuron is now no longer in the connection linked list
        assert(find_ptr_linked_list(neuron_in_next_layer, first_neuron->next_layer) == -1);

        // add the connection again
        add_connection(dummy_neural_network, 0, pos_neuron_next_layer);

        // Check that the neuron is again in the linked list
        assert(find_ptr_linked_list(neuron_in_next_layer, first_neuron->next_layer) == 0);
    }
    return;
}

/*
Tests if the neurons in the linked list are exactly the same as in the array*/
int match_neurons_linked_list_vs_array(node_linked_list_connection* act_conns, neuron** pred_conns, size_t len_arr){
    
    // Get a counter to check against len_arr to see if the right amount of connections have been made
    // This is to detect the case where the linked list is missing connections
    size_t counter = 0;

    // Loop over the neurons in the linked_list of actual connections
    while(act_conns != NULL){
        neuron* neuron_to_find = act_conns->data;

        // Pointer to the current location in the array
        size_t ptr = 0;
        // Search for the neuron in the array  
        
        while (1){

            // Case when we are about to run out of bounds
            if (ptr == len_arr){
                printf("Connection to neuron %zu not found!\n", neuron_to_find->pos);
                return OTHER_ERROR;
            }
            
            // Case when we found the connection
            if (neuron_to_find == pred_conns[ptr]){
                // Set the neuron that was found to be NULL to allow for only one find (catch ducplicate connections)
                pred_conns[ptr] = NULL;

                break;
            }

            // Else go to the next neuron
            ptr++;
        }

        // Increment the counter by one
        counter ++;

        // Continue to the next connection
        act_conns = act_conns->next;
    }

    if (counter == len_arr){
        return SUCCESSFULL_EXECUTION_CODE;
    } else{
        return OTHER_ERROR;
    }
}


/*
Toplevel function that manages the memory allocation for the testing*/
int test_neurons(void){

    // Initialise the execution code to successfull by default
    int exit_code = SUCCESSFULL_EXECUTION_CODE;

    // The initialisation of and first testing of the network array is handled by the test_seeding function
    neuron** dummy_network_array = test_seeding();

    if (dummy_network_array == NULL){
        // case where memory allocation failed
        printf("Memory allocation for the dummy network array failed.\n Could not test for errors.\n");
        return MEMORY_ALLOCATION_ERROR;
    }


    if (test_connect_neurons(dummy_network_array) != SUCCESSFULL_EXECUTION_CODE){
        // Case when this test fails
        // The error printing is handled bu the function itself
        exit_code = OTHER_ERROR;
        goto free_dummy_array;
    }

    // Test the disconnet_connect_neurons function
    test_disconnect_connect_neurons(dummy_network_array);


free_dummy_array:
    free_network(dummy_network_array);

    return exit_code;
}

int main(void){
    
    int exit_code = test_neurons();

    if (exit_code == SUCCESSFULL_EXECUTION_CODE){
        printf("All tests for neurons.c run successfully.\n");
        return 0;
    } else{
        return exit_code;
    }
}

