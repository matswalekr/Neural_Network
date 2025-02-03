/*
This file holds all the structs necessary for the Neural Network.
All structs should be defined and explained here*/

#ifndef STRUCTS_H
#define STRUCTS_H

// Manage the configurations using the config_manager.h file
#include "../../configurations/config_manager.h"


// Forward definition of neuron for other structs definition
struct neuron;


/******************************/
/* Definitions of the structs */
/******************************/



/*******************************************/
/* 1. Node of the Linked List for learning */
/*******************************************/

/*             Layout
 |------|------|------------|--------| 
 | data | next | derivative | weight | 
 |------|------|------------|--------| */

/*
Data structure to hold the connections fo one node to the next layer.
Linked list style connection for seemless inserts and deletions.
Exposes the derivative for backpropagation.
Exposes the weight with regards to the next node for feed_forward.*/
 typedef struct node_linked_list_connection{
    struct neuron* data;                      // Pointer to the data (neuron) of the node
    struct node_linked_list_connection* next; // Pointer to the next node
    double weight;                            // Associated weight of the neuron of the next layer. Used for feed forward.
    double der_weight[NUM_THREADS];           // Partial derivative with regards to this weight
 } node_linked_list_connection;

// Definition of the size of the linked list
extern const size_t size_linked_list_connection;



/***********************************/
/* 2. Neuron of the Neural Network */
/***********************************/

/*                     Layout
 |------------|------|--------------------|-----|
 | next_layer | bias | input[NUM_THREADS] | pos |
 |------------|------|--------------------|-----| */

/*
One neuron of the Neural Network. This is where the magic happens.
Has an array of inputs with one location for each thread.*/
typedef struct neuron{
    double bias;                                    // Bias of the neuron
    size_t pos;                                     // Position of the neuron in the Neural Network
    struct node_linked_list_connection* next_layer; // Connection linked list to the next layer
    double input[NUM_THREADS];                      // Every thread has its own input they are manipulating simultaneously
    double delta[NUM_THREADS];                      // Backprop delta of each thread
    double der_biases[NUM_THREADS];                 // Derivative of the current node to update the biases
}neuron;

// Definition of the size of the neuron
extern const size_t size_neuron;



/************************************/
/* 3. Data available to each thread */
/************************************/

/*      Layout  
 |------|-----------|
 | size | entries[] |
 |------|-----------|*/

/*
Information of the training data for each thread.
Input and Output arrays (each pointers to arrays of doubles of fixed size DIMENSIONS_DATA and NUM_OUTPUT).
size defines the number of input and output arrays of this thread for iteration.*/
typedef struct THREAD_DATA{
    size_t size;          // Number of entries for this particular thread
    double** input;       // Data points for this thread. Array of pointers to arrays of doubles.
    double** output;      // Data points of output for this thread. Array of pointers to arrays of doubles.
}THREAD_DATA;

extern const size_t size_thread_data;


/************************************/
/* 4. args for running each thread  */
/************************************/

/*
            Layout
 |------|-----|-------------|
 | data | pos | network_arr |
 |------|-----|-------------|*/

#include <stdalign.h>

#define CACHE_LINE_SIZE 128
/*
Data structure carrying all necessary informations for each thread.
Used for the multi-threading approach.*/
typedef struct{
    THREAD_DATA* data;     // Data structure carrying the Input and (actual) Output for this thread
    size_t pos;            // Position of the thread in the Neural Network (determines which position of the input array in the Neural Network to use)
    neuron** network_arr;  // Pointer to the array of neurons of the Neural Network
    char padding[104];     // Padding to align the struct to exactly one cache line (Prevents false sharing)
}__attribute__((aligned(CACHE_LINE_SIZE))) THREAD_ARGS;

extern const size_t size_thread_args;

/********************************************/
/* 5. Data structure to map input to output */
/********************************************/

/*      Layout  
 |---------|----------|------|------|
 | input[] | output[] | size | used |
 |---------|----------|------|------|*/

/*
Relates an input to an output.
Uses arrays of arrays for both inputs and output.
The first arrary of inputs maps to the first array of outputs etc.*/
typedef struct INPUT_OUTPUT_MAPPING{
    double** input;   // Array of arrays of doubles as inputs
    double** output;  // Array of doubles as output (note that we can only have one output in this case)
    size_t size;      // Number of input and output arrays
    size_t used;      // Number of pointers used already
}INPUT_OUTPUT_MAPPING;

// Array of doubles to hold the sum of costs for each thread 
extern double SUM_COST[NUM_THREADS];

#endif //STRUCTS_H