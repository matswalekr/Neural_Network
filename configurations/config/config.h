/* 
This file holds all the configurations of the Neural Network.
All definitions that need to be respected throughout the 
Neural Network should be defined in this .c and .h file.

Note that, if a csv is read, some of the constants will be dynamically calculated in that file.

This file is being used by all the other .c files!! Changes need
to be made carefully.
*/

#ifndef CONFIG_H  
#ifndef TEST_CONFIG_H
#define CONFIG_H

#define VERSION "NormalRunning"

// Include stddef.h to use size_t
#include <stddef.h>

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

// 1.1 Dimensions of the input variables. This is Data specific and should be changed depending on the dataset
extern size_t DIMENSIONS_DATA;

// 1.2 The size (number of data points) of the training data
extern size_t SIZE_TRAIN;

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
#define NUMBER_LAYERS (HIDDEN_LAYERS + 2) // By default there must always be 2 more total layers than hidden layers (input and output layer)

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
#define LEARNING_RATE 1e-3f

// 3.2 The number of times the Neural Network should run during the training process
#define GENERATIONS 30000

// 3.3 The name of the file from which to learn
extern const char *FILENAME;

/*
|------------------------|
| 4. Constant Functions  |
|------------------------|
*/

// 4.1 Activation function definition (a struct with activation function and its derivative)
#define ACTIVATION_FUNCTION(input) leaky_relu((input), 0.2)
#define ACTIVATION_FUNCTION_DER(input) der_leaky_relu((input), 0.2)

// 4.2 Cost function definition (a struct with activation function and its derivative) (currently not working for exponential cost)
#define COST_FUNCTION(actual, pred) mean_squared_error((actual), (pred))
#define COST_FUNCTION_DER(actual, pred) mean_squared_error_der((actual), (pred))


/*
|--------------------|
| 5. Other Constants |
|--------------------|
*/

// 5.1 Number of threads used in the multiprocessing (change to make the code faster)
#define NUM_THREADS 1
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

#endif // endif for TEST_CONFIG_H
#endif // endif for CONFIG_H