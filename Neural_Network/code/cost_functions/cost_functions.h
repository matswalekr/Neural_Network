/*
Header file to give the layout of the cost functions
Includes config.h for configurations
Is prohibited from using any other header files*/

#ifndef COST_FUNCTIONS_H
#define COST_FUNCTIONS_H

double squared_error(double**, double**, size_t);
//double mean_squared_error(double**, double**, size_t);
double mean_squared_error(double, double);
double mean_squared_error_der(double, double);

double absolute_error(double**, double**, size_t);
double mean_absolute_error(double**, double**, size_t);

double cross_entropy_cost(double**, double**, size_t);

double exponential_cost(double**, double**, float, size_t);

double kl_divergence(double**, double**, size_t);

double generalised_kl_divergence(double**, double**, size_t);
double itakura_saito_distance(double**, double**, size_t);

double calc_average_cost(void);

#endif // COST_FUNCTIONS_H
