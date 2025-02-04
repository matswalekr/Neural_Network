/* 
This file holds all the configurations for testing the Neural Network.
All definitions that need to be respected throughout the Neural Network should be defined in this .c and .h file

This file is being used by all the other .c files!! Changes need to be made carefully.
*/

#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#define VERSION "TestVersion"

// Include stddef.h to use size_t
#include <stddef.h>

// To use assert in Macro

// Include cost functions for Macro.
#include "../../code/cost_functions/cost_functions.h"

// Include activation functions for Macro
#include "../../code/activation_functions/activation_functions.h"


/*
|------------------|
| DEFINE CONSTANTS |
|------------------|
*/

/*
|------------------------------------------------------|
| 1. Constants of the Input Data of the Neural Network |
|------------------------------------------------------|
*/
// If the process_input file is included, these constants will be dynamically calculated there
#ifndef PROCESS_INPUT_H

// 1.1 Dimensions of the input variables. This is Data specific and should be changed depending on the dataset
#define DIMENSIONS_DATA 1

// 1.2 The size (number of data points) of the training data
#define SIZE_TRAIN 6
#endif // PROCESS_INPUT

#ifdef PROCESS_INPUT_H
// 1.1 Dimensions of the input variables. This is Data specific and should be changed depending on the dataset
extern size_t DIMENSIONS_DATA;

// 1.2 The size (number of data points) of the training data
extern size_t SIZE_TRAIN;
#endif // PROCESS_INPUT

// 1.3 Dimensions of the ouput layer. This is data specific and should be changed depending on the dataset
#define NUM_OUTPUT 1


/*
|---------------------------------------------------------|
| 2. Constants for the architecture of the Neural Network |
|---------------------------------------------------------|
*/

// 2.1 Number of hidden layers of the Neural Network. By default, all layers are connected
#define HIDDEN_LAYERS 0

// 2.2 Number of total layers of the Neural Network. Includes input, hidden and output layers
#define NUMBER_LAYERS (HIDDEN_LAYERS + 2) // By default there must always be 2 more total layers than hidden layers

// 2.3 Number of Neurons per Layer
extern const size_t NEURON_NUMBERS[NUMBER_LAYERS];

// 2.4 Global definition getting the length of the Neural Network (the absolute number of neurons)
extern size_t LENGTH_NETWORK;

// 2.5 Global definition of the non-hidden number of neurons (the number of connected neurons)
extern size_t WORKING_NEURONS;

/*
|---------------------------------------|
| 3. Constants for the Learning Process |
|---------------------------------------|
*/

// 3.1 The learning rate of the neural network (fine tune to get fast, but solid convergence)
#define LEARNING_RATE 1e-2f

// 3.2 The number of times the Neural Network should run during the training process
#define GENERATIONS 10


/*
|------------------------|
| 4. Constant Functions  |
|------------------------|
*/

// 4.1 Activation function definition (a struct with activation function and its derivative)
#define ACTIVATION_FUNCTION(input) relu(input)
#define ACTIVATION_FUNCTION_DER(input) 1        // For testing purposes, we simply return the input value

// 4.2 Cost function definition (a struct with activation function and its derivative)
#define COST_FUNCTION(actual, pred) mean_squared_error((actual), (pred))
#define COST_FUNCTION_DER(actual, pred) mean_squared_error_der((actual), (pred))


/*
|--------------------|
| 5. Other Constants |
|--------------------|
*/

// 5.1 Number of threads used in the multiprocessing (change to make the code faster)
#define NUM_THREADS 4
// To get the number of cores: #include <unistd.h>  int cores = sysconf(_SC_NPROCESSORS_ONLN);


/*
|----------------------|
| 5. Error Definitions |
|----------------------|
*/

// 5.0 Successfull Exectution return
#define SUCCESSFULL_EXECUTION_CODE 0

// 5.1 Exit code if the end of a CSV is reached during reading
#define END_OF_CSV 002

// 5.2 Memory Allocation Error
#define MEMORY_ALLOCATION_ERROR 200

// 5.3 Thread Creation Error
#define THREAD_CREATION_ERROR 301

// 5.4 Thread Joining Error
#define THREAD_JOINING_ERROR 302

// 5.5 Error when reading a file
#define FILE_ERROR 400

// 5.6 Other Error
#define OTHER_ERROR 500


/*
|---------------------|
| 6. Helper functions |
|---------------------|
*/

// 6.1 Tolerance for floating point arithmetic
#define TOLERANCE 1e-4

// 6.2 Comparison Macro for doubles with error message (hidden)
#define COMPARE(act, exp)                                                      \
    do {                                                                       \
        double diff = fabs((double)(act) - (double)(exp));                     \
        if (diff >= TOLERANCE) {                                               \
            fprintf(stderr,                                                    \
                    "Difference exceeded floating-point tolerance in %s:%d\n"  \
                    "Expected value: %.8f\n"                                   \
                    "Actual value: %.8f\n"                                     \
                    "Difference: %.8f\n",                                      \
                    __FILE__, __LINE__, (double)(exp), (double)(act), diff);   \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
    } while (0)

#endif // TEST_CONFIG_H