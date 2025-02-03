
# General
This file holds the code for a Neural Network algorithm written in C. Note that the code is not written in vector/matrix format, but in an "object" manner (all ideas are represented by different structs holding their info). This will have a negative effect on performance.

The file expects the input to be numeric and organised in a csv such that the "true" output is at the end of each line. It also expects that the number of output data points is the same as the number of Output Nodes in the Neural Network.


# Building using Makefile
For normal running:  
compile using **make** or **make release**  
When running it, include the path to the csv as command line argument.

For testing a certain file xyz:  
compile using **make test_xyz**  (Note that a xyz/test/test_xyz file needs to exist for this to work)

For demo with the demo file [demo.csv](code/main/demo.csv):  
compile using **make demo**

# Organisation of code
The file is organised in 3 main building blocks:
- **Configurations** 
- **Code** 
- **Builds**

## Configurations folder
The **configurations folder** holds the different configurations for the Neural Network. It is in the 
[config.c](configurations/config/config.c) 
and [config.h](configurations/config/config.h) 
files that the architecture of the Neural Network and the different constants/parameters can be accessed and changed.

**This file may be changed during normal running!**


## Code folder
The **code folder** holds the different algorithms for the Neural Network in their respective files, such as 
[feed_forward.c](code/feed_forward/feed_forward.c), [gradient_descent.c](code/gradient_descent/gradient_descent.c) or a [csv reader](code/process_input/process_input.c). 
It also exposes various cost functions and their derivatives as well as activation functions that can be used. Note that up to this point, 
the algorithm only supports one output neuron.  
As a convention, every file has its own testing file.

**During normal running, the code folder may not be touched!**

## Build folder
The build folder simply holds the executables for the neural network. It has an own section for all testing executables.


# Implementation Details

## Gradient Descent
The Neural Network uses basic gradient descent (first derivative) to update its weights and biases. For both, the learning rate is the same. During seeding, they are initialised to a random number.  
Updates are performed in a big batch once for every epoch (after the entire training data has been run for the previous Network).

## Optimisations
To speed up the process, the training data is broken into smaller pieces and processed using multi-threading. Note, that only the [feed_forward](code/feed_forward/feed_forward.c) algorithm benefits from this speed-up, as it is the main bottleneck for big training datasets.  
The code is optimised for cache sizes of 128 bytes (If necessary change CACHE_LINE_LENGTH).
