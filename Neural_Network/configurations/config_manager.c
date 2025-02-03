#include "config_manager.h"
#include <stdio.h>

size_t LENGTH_NETWORK;  // Define the variables (But change them later)
size_t WORKING_NEURONS; // Define the variables (But change them later)


inline size_t length_network(void){
    // Get the total length of the neural network
    size_t length_network_ = 0;
    for (size_t layer_number = 0; layer_number < NUMBER_LAYERS; layer_number++){
        length_network_ += NEURON_NUMBERS[layer_number];
    }
    return length_network_;
}

/*
Function to initialise the LENGTH_NETWORK and WORKING_NEURONS variables of the neural network.
Call this function as soon as the neural network is created.*/
void initialise_length_network(void){
    LENGTH_NETWORK = length_network();
    WORKING_NEURONS = LENGTH_NETWORK - NUM_OUTPUT;
}