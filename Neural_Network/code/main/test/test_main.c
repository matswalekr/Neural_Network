#include <stdio.h>
#include <stdlib.h>

#include "test_main.h"
#include "../main.h"
#include "../../../configurations/config_manager.h"

int main(void){
    printf("This file exists!\n");
    if (SUCCESSFULL_EXECUTION_CODE != run_neural_network()){
        printf("There was an error!");
    }
    return 0;
}