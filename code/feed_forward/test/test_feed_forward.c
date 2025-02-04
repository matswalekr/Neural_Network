/*
Test file for the Feed forward algorithm.
May not be included in any other files. Its own purpose is for testing.

For testing purposes:
- ACTIVATION_FUNCTION_DER(x) = x;
- COST_FUNCTION_DER(x,y) = x-y; 

*/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // For fabs

// To use free_thread_data_array
#include "../../process_input/process_input.h"
#include "test_feed_forward.h"
#include <pthread.h>


// Basic test
void test1(neuron** network_arr){

    // Manipulate the neural network such that we have easy to work with data
    set_bias(network_arr, 0, 0); // Set the bias for node 0 to 0
    set_bias(network_arr, 1, 0); // Set the bias for node 1 to 0

    network_arr[0]->next_layer->weight = 2; // Set the weight

    double input[1] = {4};
    double output[1] = {8};
    create_output(network_arr, input, output, 0);

    /*
    The output should be: 
    (4 + 0) * 2 --> 8 (input node)
    (8 + 0) --> 8 (output node)
    */

   // Use the delta to check if we got the right answer
   double delta = network_arr[1]->delta[0];

   COMPARE(delta, 0);
   return;
}

// Basic test
void test2(neuron** network_arr){

    // Manipulate the neural network such that we have easy to work with data
    set_bias(network_arr, 0, 1); // Set the bias for node 0 to 1
    set_bias(network_arr, 1, 1); // Set the bias for node 1 to 1

    network_arr[0]->next_layer->weight = 2; // Set the weight

    double input[1] = {4};
    double output[1] = {11};
    create_output(network_arr, input, output, 0);

    /*
    The output should be: 
    (4 + 1) * 2 --> 10 (input node)
    (10 + 1) --> 11 (output node)
    */

   // Use the delta to check if we got the right answer
   double delta = network_arr[1]->delta[0];

   COMPARE(delta, 0);
   return;
}

// Basic test
void test3(neuron** network_arr){

    // Manipulate the neural network such that we have easy to work with data
    set_bias(network_arr, 0, 0); // Set the bias for node 0 to 0
    set_bias(network_arr, 1, 0); // Set the bias for node 1 to 0

    network_arr[0]->next_layer->weight = (double) (-1.5); // Set the weight

    double input[1] = {8};
    double output[1] = {0};

    create_output(network_arr, input, output, 0);

    /*
    The output should be: 
    0 as we are using relu as activation function
    */

   // Use the delta to check if we got the right answer
   double delta = network_arr[1]->delta[0];

   COMPARE(delta, 0);
   return;
}

// Test for delta != 0
void test4(neuron** network_arr){

    // Manipulate the neural network such that we have easy to work with data
    set_bias(network_arr, 0, -1); // Set the bias for node 0 to -1
    set_bias(network_arr, 1, 8); // Set the bias for node 1 to 4

    network_arr[0]->next_layer->weight = -1.5; // Set the weight

    double input[1] = {-3};
    double output[1] = {4};
    create_output(network_arr, input, output, 0);

    /*
    The output should be: 
    (-3 - 1) * (-1.5) --> -6 (input node)
    (-6 + 8) --> 2 (output node) (!= 4)
    */

   // Use the delta to check if we got the right answer
   double delta = network_arr[1]->delta[0]; 

   // Calculate the delta (as the expected output is not the output of the NN)
   // cost_der = 4-2 = 2
   // Activation_der = 2
   // -> delta = 2*2 = 4

   COMPARE(delta, 4);
   return;
}

// Multiprocessing test
int test5(neuron** network_array){

    // Manipulate the neural network such that we have easy to work with data
    set_bias(network_array, 0, 0); // Set the bias for node 0 to 0
    set_bias(network_array, 1, 1); // Set the bias for node 1 to 1

    network_array[0]->next_layer->weight = 2; // Set the weight

    size_t exit_code = SUCCESSFULL_EXECUTION_CODE;
    // 3.1 Spin off the different threads
    // Array of the different threads
    pthread_t threads[NUM_THREADS];

    // Array of the different data points for each thread
    THREAD_DATA* thread_data = calloc(NUM_THREADS, sizeof(THREAD_DATA));
    if (!thread_data){
        exit_code = MEMORY_ALLOCATION_ERROR;
        perror("Memory allocation issue for thread_data.\n");
        goto exit;
    }

    // Fill the thread_data with the test data
    double input_test[NUM_THREADS] = {1,2,3,4};

    // Note that this is also the true output: (input + 0) * 2 + 1
    double output_test[NUM_THREADS] = {3,5,7,9};

    // Declare thread_pos_ outside of scope of loop
    int thread_pos_;
    for(thread_pos_ = 0; thread_pos_ < NUM_THREADS; thread_pos_++){
        // Size
        thread_data[thread_pos_].size = 1;

        // Input
        double* input = malloc(sizeof(double));
        if (!input){
            printf("Memory allocation error for input of thread  %i", thread_pos_);
            exit_code = MEMORY_ALLOCATION_ERROR;
            goto free_threads;
        }
        *input = input_test[thread_pos_];

        double** input_arr = malloc(sizeof(double*));
        if (!input_arr){
            printf("Memory allocation error for input_arr of thread  %i", thread_pos_);
            exit_code = MEMORY_ALLOCATION_ERROR;
            free(input);
            goto free_threads;
        }
        *input_arr = input;
        thread_data[thread_pos_].input = input_arr;

        // Output
        double* output = malloc(sizeof(double));
        if (!output){
            printf("Memory allocation error for output of thread  %i", thread_pos_);
            exit_code = MEMORY_ALLOCATION_ERROR;
            goto free_threads;
        }
        *output = output_test[thread_pos_];

        double** output_arr = malloc(sizeof(double*));
        if (!output_arr){
            printf("Memory allocation error for output_arr of thread  %i", thread_pos_);
            exit_code = MEMORY_ALLOCATION_ERROR;
            free(output);
            goto free_threads;
        }
        *output_arr = output;
        thread_data[thread_pos_].output = output_arr;
    }

    
    // 3.2 Set up the data for each thread to work on
    THREAD_ARGS* thread_args_arr = calloc(NUM_THREADS, size_thread_args);
    if (!thread_args_arr){
        // Case where memory allocation failed
        perror("Memory allocation error when trying to calloc memory of THREAD_ARGS array.\n");
        exit_code = MEMORY_ALLOCATION_ERROR;
        goto free_threads;
    }

    // Create variables that will be accessed repeatedly throughout the next loops.
    size_t thread_pos;   // Position of the thread we are currently at (used by multiple loops)

    // Initialise the data for each thread
    for(thread_pos = 0; thread_pos < NUM_THREADS; thread_pos++){
        thread_args_arr[thread_pos].pos = thread_pos;
        thread_args_arr[thread_pos].data = &thread_data[thread_pos]; // Pointer to the THREAD_DATA datastructure for this thread
        thread_args_arr[thread_pos].network_arr = network_array;
    }

    // 3.3 Distribute the workload among the threads and run them
    // Every step from now on can be done in a loop
    for (size_t generation = 0; generation < 1; generation++){

        // 3.3.1 Make the different threads work        
        for(thread_pos = 0; thread_pos < NUM_THREADS; thread_pos++){
            if (pthread_create(&threads[thread_pos], NULL, &work_thread, (void*) &thread_args_arr[thread_pos]) != 0){
                printf("Creation of Thread %zu was unsuccessfull in generation %zu.\n", thread_pos, generation);
                exit_code = THREAD_CREATION_ERROR;
                goto free_thread_args_arr;
            }
        }

        // 3.3.2 Join the threads back together and update the total error
        for (thread_pos = 0; thread_pos < NUM_THREADS; thread_pos++){
            if (pthread_join(threads[thread_pos], NULL) != 0){
                printf("Joining of thread %zu was unsuccessfull in generation %zu\n", thread_pos, generation);
                exit_code = THREAD_JOINING_ERROR;
                goto free_thread_args_arr;
            }
        }
        
        // Check if there was a problem with the thread behavior
        if (exit_code != SUCCESSFULL_EXECUTION_CODE){
            printf("Problems in generation %zu\n", generation);
            goto free_thread_args_arr; // terminate the execution program and start cleanup
        }
  }

    for (int i = 0; i < NUM_THREADS; i++){
        double act = network_array[1]->delta[i];
        double exp = 0; // We expect a delta of 0, as the input is the right one
        COMPARE(act, exp);
    }

free_thread_args_arr:
    free(thread_args_arr);
free_threads:
    free_thread_data_array(thread_data);
exit:
    return exit_code;
}



/*
Function that manages the Testing*/
int test_handler_func(void){

    int return_code = SUCCESSFULL_EXECUTION_CODE;

    // Create a Simple 2 layer Neural Network
    neuron** network_arr = seed_neural_network();
    if (!network_arr){
        printf("Seeding the network array was unsuccessfull in test_feed_forward.\n");
        return OTHER_ERROR;
    }
    
    // Connect the neurons
    int output = connect_neurons(network_arr);
    if(output != SUCCESSFULL_EXECUTION_CODE){
        printf("Error when connecting the neurons.\n");
        // Note that the network array will be freed in the function
        return OTHER_ERROR;
    }


    /***********************************
     * Test the create_output function *
     ***********************************/

    test1(network_arr);
    test2(network_arr);
    test3(network_arr);
    test4(network_arr);


    /*********************************
     * Test the threading capability *
     *********************************/

    if(SUCCESSFULL_EXECUTION_CODE != test5(network_arr)){
        printf("Problem with the multiprocessing.\n");
        return OTHER_ERROR;
    }


    printf("All tests for feed_forward successfully executed.\n");
    free_network(network_arr);
    return return_code;
}

int main(void){
    return test_handler_func();
}