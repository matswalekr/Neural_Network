/*
File for gradient descent of the Neural Network.
This file may change the attributes of the Neural Network in place.
It takes a sole (!) total_cost variable to update the weights.
Note that it changes the weigths in-place
*/

#include <stdio.h>
#include <stdlib.h>

#include "gradient_descent.h"

/*
Update of the parameters for one neuron.*/
inline void gradient_descent_neuron(neuron* this_neuron){

    /**********************
     * 1. Update the bias *
     **********************/

    double bias_der = 0;
    // Sum up all the biases calculated from the different threads
    for (size_t i = 0; i < NUM_THREADS; i++){
        bias_der += this_neuron->der_biases[i];
        this_neuron->der_biases[i] = 0;                             // Reset the derivative to 0
    }

    double new_bias = this_neuron->bias - LEARNING_RATE * (bias_der / SIZE_TRAIN); // Updated bias

    this_neuron->bias = new_bias;

    // Set the input equal to the bias (design choice)
    for (size_t i = 0; i < NUM_THREADS; i++){
        this_neuron->input[i] = new_bias;
    }

    /*************************
     * 2. Update the weigths *
     *************************/

    node_linked_list_connection* con = this_neuron->next_layer;
    while(con){
        double weight_der = 0;

        for (size_t i = 0; i < NUM_THREADS; i++){
            weight_der += con->der_weight[i];
            con->der_weight[i] = 0;                             // Reset the derivative to 0
        }
        con->weight -= (weight_der / SIZE_TRAIN) * LEARNING_RATE;

        con = con->next;
    }
}

/*
Gradient descent function to make the Neural Network learn.
Note that this updates the Neural Network in-place.
Nothing is returned.
No errors will be caught.*/
void gradient_descent(neuron** network_arr){

    // 1. Update all input/ hidden layers
    size_t pos;
    for (pos = 0; pos < WORKING_NEURONS; pos++){
        gradient_descent_neuron(network_arr[pos]);
    }

    // 2. Update all the output layers (only bias)
    for (size_t i = 0; i < NUM_OUTPUT; i++){
        neuron* output_neuron = network_arr[pos];

        double bias_der = 0;
        // Sum up all the biases calculated from the different threads
        for (size_t i = 0; i < NUM_THREADS; i++){
            bias_der += output_neuron->der_biases[i];
            output_neuron->der_biases[i] = 0;                             // Reset the derivative to 0
        }

        double new_bias = output_neuron->bias - LEARNING_RATE * (bias_der / SIZE_TRAIN); // Updated bias

        output_neuron->bias = new_bias;

        // Set the input equal to the bias (design choice)
        for (size_t i = 0; i < NUM_THREADS; i++){
            output_neuron->input[i] = new_bias;
        }

        pos++;
    }
}
