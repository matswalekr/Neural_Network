/*
This file holds the functions for the activation of the Neural Network (and their derivatives)
The derivatives of a function act_func() is not as der_act_func()
This file may not use any header files but its own
Uses math.h for calculations.

Multiple different functions can be chosen from:
-sigmoid
-tanh
-relu
-leaku_relu
*/

#include "activation_functions.h"
#include <math.h>

double sigmoid(double number){
    /*
    Used to predict the probability as an output, returns floats between 0 and 1
    - vanishing-gradient problem (f'(x)->0 for x->+-infinity)
    Monotonic rising
    Differentiable (but not monotonic)*/
    return (double) 1/(1 + exp(-number));
}

double der_sigmoid(double number){
    /*
    Derivative of the sigmoid activation function
    Used for backpropagation*/
    return sigmoid(number) * (1- sigmoid(number));
}

double tanh_(double number){
    /*
    Used for classification between classses, returns floats between -1 and 1
    Maps negative inputs as strongly negative and vice versa for positive
    + Symmetric around 0 (leads to faster convergence)
    - Vanishing-gradient problem (f'(x)->0 for x->+-infinity)
    Monotonic rising
    Differentiable (but not monotonic)tanh(number);*/
    return (double)tanh(number);
}

double der_tanh_(double number){
    /*
    Derivative of the tanh activation function
    Used for backpropagation*/
    double tanh_value = tanh(number);

    // Derivative of tanh(x) = 1 - tanh^2(x)
    return 1 - tanh_value * tanh_value;
}

double relu(double number){
    /*
    Used in convolutional NN and deep learning
    Returns floats between 0 and infinity
    + Solves vanishing-gradient problem (f'(x)->0 for x->+-infinity)
    + Light-weight function, allows for many Nodes
    - Does not really learn from negative inputs
    Monotonic rising
    Non-differentiable at 0 (monotonic else)*/
    if (number > 0){
        return number;
    }
    return 0;
}

double der_relu(double number){
    /*
    Derivative of the relu activation function
    Used for backpropagation
    Note that the relu function is non differentiable in 0.
    Fore ease the derivative at 0 is defined as 0.5*/
    if (number > 0){
        return 1;
    }
    if (number == 0){
        return 0.5;
    }
    return 0;
}

double leaky_relu(double number, double n ){
    /*
    Used in convolutional NN and deep lerning
    Returns floats between 0 and infinity
    + Solves problem from relu activation function
    Monotonic rising
    Non-differentiable at 0 (monotonic else)*/
    if (number > 0){
        return number;
    }
    return number * n;
}

double der_leaky_relu(double number, double n){
    /*
    Derivative of the leaky relu activation function
    Used for backpropagation
    Note that the leaky relu function is non differentiable in 0.
    Fore ease the derivative at 0 is defined as (1+n)/2*/
    if (number > 0){
        return 1;
    }
    if (number == 0){
        return (1+n)/2;
    }
    return n;
}
