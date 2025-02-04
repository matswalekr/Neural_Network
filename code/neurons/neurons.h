/*
This file holds the code necessary for initialising the Neural Newtork structure
The entire architecture should be managed by this file.
*/

#ifndef NEURONS_H
#define NEURONS_H


#include <stddef.h>
#include "../structs/structs.h"


// Functions declared in this file
extern inline neuron* create_new_neuron(size_t);


neuron** seed_neural_network(void);
int connect_neurons(neuron**);

int del_connection(neuron**, size_t, size_t);
int add_connection(neuron**, size_t, size_t);

void set_bias(neuron**, size_t, double);

float random_number(void);
void seed_random(void);

void free_network(neuron**);
void free_linked_list(node_linked_list_connection*);

#endif //NEURONS_H