/*
Header file of the feed forward file.
Includes structs.h to use THREAD_ARGS and THREAD_DATA*/

#ifndef FEED_FORWARD_H
#define FEED_FORWARD_H

#include "../structs/structs.h"
#include "../cost_functions/cost_functions.h" // Include cost functions 

void* work_thread(void*);
void create_output(neuron**, double[DIMENSIONS_DATA], double[NUM_OUTPUT], size_t);

void distribute_input_data(neuron**, double[DIMENSIONS_DATA], size_t);
void forward_pass(neuron**, double[NUM_OUTPUT], size_t);
void backward_pass(neuron**, size_t);

#endif // FEED_FORWARD_H