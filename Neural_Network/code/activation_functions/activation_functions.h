/*
Header file for the activation functions of the Neural Network.
Should only include the outlay of these functions.
Should not use any outside libraries.

Includes activation functions and their derivatives.
Include:
-sigmoid
-tanh
-relu
-leaku_relu
*/


#ifndef ACTIVATION_FUNCTIONS_H
#define ACTIVATION_FUNCTIONS_H

double sigmoid(double);
double der_sigmoid(double);

double tanh_(double);
double der_tanh_(double);

double relu(double);
double der_relu(double);

double leaky_relu(double, double);
double der_leaky_relu(double,double);


#endif //ACTIVATION_FUNCTIONS_H