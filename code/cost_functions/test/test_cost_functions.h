#ifndef TEST_COST_FUNCTIONS_H
#define TEST_COST_FUNCTIONS_H

#include "../../../configurations/test_config/test_config.h"
#include "../cost_functions.h"

int test_mean_squared_error();
int test_mean_absolute_error();
int test_cross_entropy_cost();
int test_exponential_cost();
int test_kl_divergence();
int test_generalised_kl_divergence();
int test_itakura_saito_distance();

#endif //TEST_COST_FUNCTIONS_H