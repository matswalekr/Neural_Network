/*
File responsible for the Feed Forward logic of the Neural Network
May not alter or interfere with the architecture of the Neural Network
Uses multithreading for faster execution

Includes pthread.h for multithreading
Includes config.h for the different configurations of the model*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Include pthread for pthread_exit

#include "feed_forward.h"
#include "../helper_functions/helper_functions.h"

/*
This function distributes the input data to the input nodes*/
inline void distribute_input_data(neuron** neuron_arr, double input_data[DIMENSIONS_DATA], size_t thread_pos){
    size_t pos = 0;                                   // Pointer to the location of the first neuron in the Neural Network array
    size_t number_input_neurons = NEURON_NUMBERS[0];  // Number of neurons in the input layer

    // Use this "double loop" to manage the case when there are more dimensions than input neurons (we will just add the data up together)
    for (size_t ptr_dimension = 0; ptr_dimension < DIMENSIONS_DATA; ptr_dimension++){

        // case when we have used all neurons, but still have input data
        if (pos == number_input_neurons){
            pos = 0; // Go back to the first input neurons
        }

        neuron_arr[pos]->input[thread_pos] += input_data[ptr_dimension];
        // Increment the position to go to the next neurons
        pos++;
    }

    return;
}


/*
This function manages the forward pass of the data through the neural network.
Note that it already calculates the deltas for the output neurons*/
inline void forward_pass(neuron** neuron_arr, double output_data[NUM_OUTPUT], size_t thread_pos){

    int pos;

    // Set the SUM_COST to 0
    SUM_COST[thread_pos] = 0;

    // Iterate over all connected neurons in the array
    for (pos = 0; pos < (int) WORKING_NEURONS; pos++){
        neuron* curr_neuron = neuron_arr[pos];

        // Calculate the output
        double output = ACTIVATION_FUNCTION(curr_neuron->input[thread_pos]);

        // Propagate the output to the next layer
        node_linked_list_connection* next_layer = curr_neuron->next_layer;
        while(next_layer){
            neuron* next_neuron = next_layer->data;
            next_neuron->input[thread_pos] += next_layer->weight * output; // Add the respective value to the input of the next neuron
            next_layer = next_layer->next;                                 // Move to the next neuron
        }
    }

    // Get the different outputs from each output neuron and calculate their respective derivatives
    for (size_t output_ptr = 0; output_ptr < NUM_OUTPUT; output_ptr++){
        neuron* output_neuron = neuron_arr[pos];

        double pred_output = ACTIVATION_FUNCTION(output_neuron->input[thread_pos]); // Calculate the "actual" output of this node

        double act_output = output_data[output_ptr];                                // Retrieve the value of the output from the training data

        double cost_der = COST_FUNCTION_DER(act_output, pred_output);               // Calculate the cost derivative associated to this output neuron

        double delta = cost_der * ACTIVATION_FUNCTION_DER(pred_output);             // Calculate the delta of the neuron
        
        SUM_COST[thread_pos] += COST_FUNCTION(act_output, pred_output);             // Update the SUM_COST to visualize the effectiveness of the Neural Network later

        output_neuron->delta[thread_pos] = delta;                                   // Update the delta arr
        output_neuron->der_biases[thread_pos] += delta;                             // Add the delta to the accumulated derivative of the bias

        output_neuron->input[thread_pos] = output_neuron->bias;                     // Reset the input to the bias

        pos++;
    }
    return; 
}


/*
Backwards pass through the Neural Network to calculate the derivatives and deltas for each node.
Note that no change may be done in this function to the actual neuron itself.*/
void backward_pass(neuron** neuron_arr, size_t thread_pos){

    double cum_delta; // Double to carry all the deltas from the next layers

    // Walk backwards through the connected layers to update their deltas
    for (int pos = (WORKING_NEURONS - 1); pos >= 0; pos--){
        neuron* curr_neuron = neuron_arr[pos];

        cum_delta = 0; // Reinitialise the cum_delta to be 0

        // Get the deltas from all connected neurons in the next layer
        node_linked_list_connection* next_layer = curr_neuron->next_layer;
        while(next_layer){
            cum_delta += next_layer->data->delta[thread_pos] * next_layer->weight;
            next_layer = next_layer->next;
        }

        // Update the delta of this neuron
        double delta = cum_delta * ACTIVATION_FUNCTION_DER(curr_neuron->input[thread_pos]);
        curr_neuron->delta[thread_pos] = delta;
        curr_neuron->der_biases[thread_pos] += delta;                                     // Add the delta to the accumulated derivative of the bias

        // Update the derivatives for the weights
        double output_this_neuron = ACTIVATION_FUNCTION(curr_neuron->input[thread_pos]);  // Activated output of this neuron

        // Iterate over all connected neurons
        node_linked_list_connection* con = curr_neuron->next_layer;
        while (con){
            con->der_weight[thread_pos] += output_this_neuron * con->data->delta[thread_pos];
            con = con->next;
        }

        curr_neuron->input[thread_pos] = curr_neuron->bias;                               // Reset the input field to the bias
    }
    return;
}


/*
This function calculates the transformation of the input data to the output with the current Neural Network*/
inline void create_output(neuron** neuron_arr, double input_data[DIMENSIONS_DATA], double output_data[NUM_OUTPUT], size_t thread_pos){

    /***************************************************
     * 1. Distribute the input data to the input nodes *
     ***************************************************/
    
    distribute_input_data(neuron_arr, input_data, thread_pos);

    /*******************************************************************
     * 2. Continue the flow of input->output through the hidden layers *
    ********************************************************************/

    forward_pass(neuron_arr, output_data, thread_pos);

    /******************************************************
     * 3. Calculate the respective deltas and derivatives *
     ******************************************************/

    backward_pass(neuron_arr, thread_pos);

    return;
}


/*
Work of one thread
Note that this function assumes that the output is one dimensional
This returns the cost calculated for its data.
As such, no mean_cost_function() should be used.
*/
void* work_thread(void* args){

    double* total_cost = malloc(sizeof(double)); // Will be freed in the main.c function after being returned
    if (!total_cost){
        // Case if memory allocation fails
        perror("Failure to allocate memory for a double in a thread.\n");
        pthread_exit(NULL); // Exit the thread with NULL to indicate an error
    }
    
    // Cast thread args to the right type
    THREAD_ARGS* thread_args = (THREAD_ARGS*) args;

    // Unpack the thread_args
    THREAD_DATA* data_for_thread = thread_args->data;
    size_t pos_thread = thread_args->pos;              // Position of the thread in the Neural Network and Thread array
    neuron** network_arr = thread_args->network_arr;   // Pointer to the Neural Network
    
    // Unpack the thread data
    size_t num_data = data_for_thread->size;            // Number of input-output pairs of this thread
    double** input_arr = data_for_thread->input;        // Array[num_data] of arrays[DIMENSIONS_INPUT] of input doubles
    double** output_arr_true = data_for_thread->output; // Array of arrays[num_data] of output[NUM_OUTPUT] doubles

    // Case if the thread has no work to do
    if (num_data == 0){
        return SUCCESSFULL_EXECUTION_CODE;
    }

    for (size_t data_ptr = 0; data_ptr < num_data; data_ptr++){
        create_output(network_arr,input_arr[data_ptr], output_arr_true[data_ptr], pos_thread);
    }

    return SUCCESSFULL_EXECUTION_CODE;
}