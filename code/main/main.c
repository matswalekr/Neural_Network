#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
// Include pthread here, as it is spun of here.
#include <float.h>
// For max double value of the system

#include "main.h"
#include "../feed_forward/feed_forward.h"
#include "../gradient_descent/gradient_descent.h"
#include "../helper_functions/helper_functions.h"
#include "../neurons/neurons.h"
#include "../save_state/save_state.h"
#include "../process_input/process_input.h"
#include "../feed_forward/feed_forward.h"
#include "../../configurations/config_manager.h"


/*
This function processes the command line arguments.
Used to get the file on which to train.*/
int process_command_args(int argc, char *argv[]){
    if (argc < 2){
        printf("No file to read given.\nUse the Neural Network with %s <filename>\n", argv[0]);
        return FILE_ERROR;
    }
    FILENAME = argv[1];
    return SUCCESSFULL_EXECUTION_CODE;
}


int run_neural_network(void){

    // Main exit code (returned to main)
    size_t exit_code = SUCCESSFULL_EXECUTION_CODE;

    // Min average cost to find the best value
    double min_av_cost = DBL_MAX;
    // Generation of the min average cost
    int best_generation = 0;

/********************************************
 * 1. Build the Neural Network architecture *
 ********************************************/

    // 1.1 Seed the Neural Network
    neuron** network_array = seed_neural_network();

    if (network_array == NULL){
        //Memory allocation error
        perror("Memory allocation error for the  network array");
        exit_code = MEMORY_ALLOCATION_ERROR;
        goto exit;
    }

    // 1.2 Create the Output Node and Initialise it
    // Node whose only purpose is to receive the output of the neural network in its input field
    neuron* output_neuron = (neuron*) malloc(size_neuron); // We use a neuron (or a neuron array) to receive the output of they layer
    if (output_neuron == NULL){
        // Case where memory allocation error occurs
        perror("Memory allocation error for the output neuron.\n");
        exit_code = MEMORY_ALLOCATION_ERROR;
        goto free_network;
    }

    // 1.3 Connect the Neurons to complete the building of the Neural Network itself
    if (connect_neurons(network_array) != SUCCESSFULL_EXECUTION_CODE){
        // Case when there was a problem when connecting the neurons
        perror("Unsuccessfull connection of the network array.\n");
        exit_code = OTHER_ERROR;
        goto free_network;
    }

    // 1.4 Create the datastructure to save the state of the Neural Network
    STATE* best_state = initialise_state(network_array);
    if (!best_state){
        // Case when memory allocation error occurs
        perror("Memory allocation error for the best_state.\n");
        exit_code = MEMORY_ALLOCATION_ERROR;
        goto free_network;
    }

/************************************
 * 2. Read in the data from the CSV *
 ************************************/

    // 2.1 Open the file and read in the entire data into an INPUT_OUTPUT_MAPPING
    INPUT_OUTPUT_MAPPING* data_of_file = parse_csv(FILENAME);
    if(!data_of_file){
        printf("Could not read in the data of the csv %s.\n", FILENAME);
        exit_code = OTHER_ERROR;
        goto free_state;
    }

    // 2.2 Distribute the data among the threads. (Note that this will free the INPUT_OUTPUT_MAPPING)
    THREAD_DATA* thread_data = distribute_data_among_threads(data_of_file);
    if (!thread_data){
        perror("Could not distribute the data to the different threads.\n");
        exit_code = OTHER_ERROR;
        free_input_output_mapping(data_of_file); // In this case we will still free the INPUT_OUTPUT_MAPPING as the function has failed.
        goto free_state;                         // Free the state and return
    }

/*******************************************************************************
 * 3. Feed the data into the Neural Network and run the Feed Forward algorithm *
 *******************************************************************************/

    // 3.1 Spin off the different threads
    // Array of the different threads
    pthread_t threads[NUM_THREADS];

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
        thread_args_arr[thread_pos].data = &thread_data[thread_pos]; //Pointer to the THREAD_DATA datastructure for this thread
        thread_args_arr[thread_pos].network_arr = network_array;
    }

    // 3.3 Distribute the workload among the threads and run them
    // Every step from now on can be done in a loop
    for (size_t generation = 0; generation < GENERATIONS; generation++){


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


/***************************************************************************************************
 * 4. Run the backwards propagation algorithm to update the variables and train the Neural Network *
 ***************************************************************************************************/
        gradient_descent(network_array);

        // Calculate the sum of the different costs
        double average_cost = calc_average_cost();
        printf("The average cost of generation %zu was %f\n", generation, average_cost);
        if (average_cost < min_av_cost){
            min_av_cost = average_cost;
            best_generation = generation;
            save_state(network_array,best_state); // Save the state
        }
    }

    printf("\nThe minimum average cost was %f in generation %i.\n", min_av_cost, best_generation);

    // Restore the best state
    restore_state(network_array, best_state);

    printf("Best Network:\n\n");
    print_network(network_array);

free_thread_args_arr:
    free(thread_args_arr);
free_threads:
    free_thread_data_array(thread_data);
free_state:
    free_state(best_state);
free_network:
    free_network(network_array);
exit:
    return exit_code;
}


int main(int argc, char *argv[]){

    // Check if the name of the file is supposed to come from the command line
    #if  !defined(DEMO) && !defined(DEBUG)
    if (process_command_args(argc, argv) != SUCCESSFULL_EXECUTION_CODE){
        return FILE_ERROR;
    }
    #else
    (void) argc;  // Shut "argc unused" warning
    (void) argv;  // Shut "argv unused" warning
    #endif // DEMO && DEBUG

    #ifdef PERFORMANCE_FLAG
    return TIME_THIS(run_neural_network());
    #else
    return run_neural_network();
    #endif // PERFORMANCE_REVIEW
}