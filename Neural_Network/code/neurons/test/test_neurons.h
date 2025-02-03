#ifndef TEST_NEURONS
#define TEST_NEURONS

// Header file of the test_neurons

#ifndef STRUCTS_H
#include "../../structs/structs.h"
#endif //STRUCTS_H


// Function prototypes
neuron** test_seeding(void);
int test_connect_neurons(neuron**);
void test_disconnect_connect_neurons(neuron**);

int match_neurons_linked_list_vs_array(node_linked_list_connection*, neuron**, size_t);

int test_neurons(void);

#endif // TEST_NEURONS
