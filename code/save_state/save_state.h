/*
Header file to save the state of a Neural Network
*/

#ifndef SAVE_STATE_H
#define SAVE_STATE_H

#include "../structs/structs.h"

STATE* initialise_state(neuron**);

void save_state(neuron**, STATE*);

void restore_state(neuron**, STATE*);

void free_state(STATE*);

#endif // SAVE_STATE_H