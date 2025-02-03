/*
This file runs the definitions of its .h file and is used to 
define the necessary structs used in the programs (cant be done in .h)

This is its whole purpose and it may not be used for any other definitions.
*/

#include "config.h"
#include <stdio.h>
#include <unistd.h>

// Structs actual definition
const size_t NEURON_NUMBERS[NUMBER_LAYERS] = {3, NUM_OUTPUT};


// Declare the name of the file if necessary
#if defined(DEMO) || defined(DEBUG)
const char *FILENAME = "/Users/matswalker/Neural_Network/code/main/demo.csv";
#else
const char *FILENAME = NULL; // Set it to NULL at global scope and modiy later
#endif // DEMO