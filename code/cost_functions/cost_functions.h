/*
Header file to give the layout of the cost functions
Includes config.h for configurations
Is prohibited from using any other header files*/

#ifndef COST_FUNCTIONS_H
#define COST_FUNCTIONS_H


double mean_squared_error(double, double);

double mean_squared_error_der(double, double);



double absolute_error(double, double);

double absolute_error_der(double, double);



double cross_entropy_cost(double, double);

double cross_entropy_cost_der(double, double);



double exponential_cost(double, double);

double exponential_cost_der(double, double);


double kl_divergence(double, double);

double kl_divergence_der(double, double);


double calc_average_cost(void);

#endif // COST_FUNCTIONS_H
