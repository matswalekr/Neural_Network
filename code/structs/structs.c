/*
This file is simply responsivle for calculating the sizes of the structs defined in its header file.
No other calculations may be done in this file*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "structs.h"

// Constants to avoid calculating sizeof over and over
const size_t size_neuron                 = sizeof(neuron);
const size_t size_linked_list_connection = sizeof(node_linked_list_connection);
const size_t size_thread_data            = sizeof(THREAD_DATA);
const size_t size_thread_args            = sizeof(THREAD_ARGS);

// Array of doubles to hold the sum of costs for each thread 
double SUM_COST[NUM_THREADS];
