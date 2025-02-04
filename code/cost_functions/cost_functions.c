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

double mean_squared_error_der(double act, double pred){
    return (pred - act);
}


double squared_error(double** actual_output, double** pred_output, size_t num_input){
    // Mean Squared Error
    // Used for regression
    //+ Turns individual errors into absolute values by squaring them
    //+ Large errors get amplified by squaring, while smaller ones get reduced.
    // Version returning the absolute error (suited for multithreading)
    

    double total_error = 0;

    for (size_t i = 0; i < num_input; i++) {
        for (size_t output_ptr = 0; output_ptr < NUM_OUTPUT; output_ptr++){
            double predicted = pred_output[i][output_ptr];
            double actual = actual_output[i][output_ptr];
            double error = actual - predicted;
            total_error += error * error;
        }
    }
    return total_error;
}

double mean_squared_error(double act, double pred){
    double error = act - pred;
    return (error * error) / 2;
}
/*
double mean_squared_error(double** actual_output, double** pred_output, size_t num_input) {
    // Mean Squared Error
    // Used for regression
    //+ Turns individual errors into absolute values by squaring them
    //+ Large errors get amplified by squaring, while smaller ones get reduced
    // Version returning the mean squared error (not suited for multithreading)

    return squared_error(actual_output, pred_output, num_input) / num_input;
}
*/

double absolute_error(double** actual_output, double** pred_output, size_t num_input){
    // Mean Absolute Error
    //+ Turns individual errors into their absolute values
    //- Very slow learning process compared to MSE
    // Version returning the absolute error (suited for multithreading)
    double total_error = 0;

    for(size_t i = 0; i < num_input; i++){
        for (size_t output_ptr = 0; output_ptr < NUM_OUTPUT; output_ptr++){
            double predicted = pred_output[i][output_ptr];
            double actual = actual_output[i][output_ptr];
            double error = actual - predicted;
            total_error += fabs(error);
        }
    }
    return total_error;
}

double mean_absolute_error(double** actual_output, double** pred_output, size_t num_input){
    // Mean Absolute Error
    //+ Turns individual errors into their absolute values
    //- Very slow learning process compared to MSE
    // Version returning the mean absolute error (not suited for multithreading)

    return absolute_error(actual_output, pred_output, num_input)/ num_input;
}

double cross_entropy_cost(double** actual_output, double** pred_output, size_t num_input){
    // Cross Entropy Cost
    // Used for classification
    double total_cost = 0.0;

    for(size_t i = 0; i < num_input; i++){
        for (size_t output_ptr = 0; output_ptr < NUM_OUTPUT; output_ptr++){
            double p = fmax(fmin(pred_output[i][output_ptr], 1.0 - 1e-12), 1e-12); // clamp
            double y = actual_output[i][output_ptr];

            total_cost += y * log(p) + (1 - y) * log(1 - p);
        }
    }
    return -total_cost; // or -total_cost / SIZE_TRAIN, if you want average
}

double exponential_cost(double** actual_output, double** pred_output, float n, size_t num_input){
    // Exponential Cost
    double total_squared_error = 0;

    for(size_t i = 0; i < num_input; i++){
        for (size_t output_ptr = 0; output_ptr < NUM_OUTPUT; output_ptr++){
            double predicted = pred_output[i][output_ptr];
            double actual = actual_output[i][output_ptr];
            double error = actual - predicted;
            total_squared_error += error * error;
        }
    }
    return fabs(n * exp((1/n) * total_squared_error));
}

double kl_divergence(double** actual_output, double** pred_output, size_t num_input){
    //Kullback-Leiber Divergence
    double total_cost = 0;

    for(size_t i = 0; i < num_input; i++){
        for (size_t output_ptr = 0; output_ptr < NUM_OUTPUT; output_ptr++){
            double predicted = pred_output[i][output_ptr];
            double actual = actual_output[i][output_ptr];
            total_cost += (float) (actual * log(actual/predicted));
        }
    }
    return fabs(total_cost);
}

double generalised_kl_divergence(double** actual_output, double** pred_output, size_t num_input){
    //Generalised Kullback-Leiber Divergence
    double total_kl = 0;
    double total_actual = 0;
    double total_pred = 0;

    for(size_t i = 0; i < num_input; i++){
        for (size_t output_ptr = 0; output_ptr < NUM_OUTPUT; output_ptr++){
            double predicted = pred_output[i][output_ptr];
            double actual = actual_output[i][output_ptr];
            total_kl += actual * log(actual/predicted);
            total_actual += actual;
            total_pred += predicted;
        }
    }
    return fabs(total_kl - total_actual - total_pred);
}

double itakura_saito_distance(double** actual_output, double** pred_output, size_t num_input){
    // Itakura-Saito Distance
    double total_cost = 0;

    for(size_t i = 0; i < num_input; i++){
        for (size_t output_ptr = 0; output_ptr < NUM_OUTPUT; output_ptr++){

            double predicted = pred_output[i][output_ptr];
            double actual = actual_output[i][output_ptr];
            total_cost += actual/predicted - log(actual/predicted) - 1;
        }
    }
    return fabs(total_cost);
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
