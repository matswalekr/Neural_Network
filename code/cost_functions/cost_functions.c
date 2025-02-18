/*
Makes available different cost functions for the Neural Network.

Includes config.h for configurations.
Uses math.h for mathematical calculations.
Is prohibited from using any other header files.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "cost_functions.h"

// Include configurations for access to constants
#include "../../configurations/config_manager.h"

// Include structs for access to SUM_COST
#include "../structs/structs.h"


double mean_squared_error(double act, double pred){
    // Mean Squared Error
    // Used for regression
    //+ Turns individual errors into absolute values by squaring them
    //+ Large errors get amplified by squaring, while smaller ones get reduced.
    double error = act - pred;
    return (error * error) / 2;
}

double mean_squared_error_der(double act, double pred){
    return (pred - act);
}



double absolute_error(double act, double pred){
    // Absolute Error
    //+ Turns individual errors into their absolute values
    //- Very slow learning process compared to MSE
    // Version returning the absolute error (suited for multithreading)
    return (act - pred);
}

double absolute_error_der(double act, double pred){
    if (act > pred){
        return -1;
    }
    if (act < pred){
        return 1;
    }
    // If act == pred, even if undefined, the derivative is defined as 0
    return 0; 
} 



double cross_entropy_cost(double act, double pred){
    // Cross Entropy Cost
    // Used for classification
    // As a result, act can only take the value 0 or 1
     if (act == 0){
        return -log(1 - pred);
     }
     if (act == 1){
        return -log(pred);
     }
     // Else, nor 0 or 1 have been entered --> error condition
     perror("A value different than 0 or 1 was passed to the cross entropy cost designed for classification.\n");
     exit(OTHER_ERROR);
}

double cross_entropy_cost_der(double act, double pred){
         if (act == 0){
        return -1/(1-pred);
     }
     if (act == 1){
        return -1/pred;
     }
     // Else, nor 0 or 1 have been entered --> error condition
     perror("A value different than 0 or 1 was passed to the cross-entropy cost derivative function designed for classification.\n");
     exit(OTHER_ERROR);
}



double exponential_cost(double act, double pred){
    // Exponential Cost
    // Of form e^(-pred*act) 
   return exp(-pred*act);
}

double exponential_cost_der(double act, double pred){
    return -act * exponential_cost(act, pred);
}


double kl_divergence(double act, double pred){
    //Kullback-Leiber Divergence
    return act * log(act/pred);
}

double kl_divergence_der(double act, double pred){
    return -(act/pred) + ((1-act)/(1-pred));
}




/*
Function to calculate the average cost of the neural network*/
double calc_average_cost(void){
    double average_cost_ = 0;
    for(size_t i = 0; i < NUM_THREADS; i++){
        average_cost_ += SUM_COST[i];
    }
    return average_cost_ / SIZE_TRAIN;
}
