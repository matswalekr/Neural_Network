/*
File to test the processing of data
Works with csv files manually written into this file

May not be included in any files.*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#include "test_process_input.h"
#include "../process_input.h"
#include "../../structs/structs.h"
// Use helper functions for print_mapping
#include "../../helper_functions/helper_functions.h"

/*
Function to test the parsing of csv*/
int test_parse_csv(const char* file){
    INPUT_OUTPUT_MAPPING* mapping = parse_csv(file);
    if(!mapping){
        printf("Could not assign the mapping\n");
        return OTHER_ERROR;  
    }
    THREAD_DATA* thread_data = distribute_data_among_threads(mapping);
    if (!thread_data){
        printf("Could not assign the thread_data\n");
        free_input_output_mapping(mapping);
        return OTHER_ERROR;
    }

    print_data_threads(thread_data);

    // Free the data of the thread
    free_thread_data_array(thread_data);

    return SUCCESSFULL_EXECUTION_CODE;
}

int main(void){
    //printf("Size of double ptr: %zu\n", sizeof(double*));
    const char current_file[] = "/Users/matswalker/Neural_Network/code/process_input/test/test_file.csv";
    if (test_parse_csv(current_file) == SUCCESSFULL_EXECUTION_CODE){
        return 0;
    } else{
        return OTHER_ERROR;
    }
}