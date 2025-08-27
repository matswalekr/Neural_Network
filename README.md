
# General
This respository contains the code for a Neural Network algorithm written in C. 

The file expects the input to be numeric and organised in a .csv file, where each row corresponds to one training example with the final column being the ground-truth label to predict. The number of output data points must match the number of output nodes specified in the configuration file.

# Building using Makefile
Standard built:

compile using **make** or **make release**  
When running it, include the path to the csv as command line argument.


Testing a module xyz:  
compile using **make test_xyz**  (Note that a code/xyz/test/test_xyz file needs to exist for this to work)

Demonstrating with the demo data [demo.csv](code/main/demo.csv):  
compile using **make demo**

# Organisation of code
The project contains 3 main building blocks:
- **Configurations** - defines the architecture and parameters
- **Code** - holds the logic for the algorithmic implementation and CSV parser
- **Builds** - stores the compiled executables

# Implementation
Unlike the common vectorised implementations, this project employs a "object-oriented style". Conceptual entities are stored in their own structs. While this improves expressiveness and modularity, it reduces performance. 

## Configurations folder
- Located in [config.c](configurations/config/config.c) and [config.h](configurations/config/config.h) 
- Defines the architecture, constants and hyperparameters of the Neural Network
- Changes may be made to parameters to adapt the model


## Code folder
- Implements the necessary algorithms, including [feed_forward.c](code/feed_forward/feed_forward.c), [gradient_descent.c](code/gradient_descent/gradient_descent.c) or a [csv reader](code/process_input/process_input.c) 
- Provides multiple activation and cost functions along with their derivatives
- Current limitation: Only one output neuron is supported
- Each file should include its dedicated test file

Note that this file should not be changed during normal excecution.

## Build folder
- Contains the compiled executables
- Includes dedicated directory for testing executables


# Implementation Details

## Gradient Descent
- Optimisation is made using first order gradient descent
- A uniform learning rate is used for both weights and biases
- Parameters are initialised randomly during seeding
- Updates to these parameters are performed in batches


## Optimisations
- Training data is partitioned into small batches for parallel execution
- Multi-threading is implemented for the [feed_forward](code/feed_forward/feed_forward.c) algorithm, as it is the main bottleneck
- Memory usage is optimised for CPU cache lines of 128 bytes (configurable via CACHE_LINE_LENGTH in [config.h](configurations/config/config.h))

# Available functions

## Activation Functions
The following functions and their derivatives are available as activation functions:

- **sigmoid**
- **tanh**
- **relu**
- **leaky relu**

## Cost Functions
The following functions and their derivatives are available as cost functions:

- **mean squared error**
- **absolute error**
- **cross-entropy cost**
- **exponential cost**
- **KL divergence**