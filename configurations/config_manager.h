/*
This file manages the inclusion of the configuration files.

No code should be written here, the file just selects the configurations file based on the mode of running the Neural Network

Either includes  config/config.h for normal running or test_config/test_config.h for testing*/

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

//#ifdef TEST_MODE_FEED_FORWARD// Case if we are testing the feed forward
//#include "test_config/feed_forward_test_config.h"

//#elif defined(TEST_MODE) // Case when we are testing normally
#ifdef TEST_MODE
#include "test_config/test_config.h"

#else // Case when we are normally running the file
#include "config/config.h"
#endif // TEST_MODE

/*
|-------------------|
|  Helper functions |
|-------------------|
*/

void initialise_length_network(void);
size_t length_network(void);

#endif // CONFIG_MANAGER_H