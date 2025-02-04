###############################################################################
# Compiler and Flags
###############################################################################
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread # Include lpthread for multithreading

.PHONY: all release debug demo clean

# Debug, Test and Release Flags
DEBUG_FLAGS = -g -O0 -fno-inline -DDEBUG
TEST_FLAGS = $(DEBUG_FLAGS) -DTEST_MODE
RELEASE_FLAGS = -O3 -march=native -flto -fno-omit-frame-pointer -funroll-loops
DEMO_FLAGS = $(RELEASE_FLAGS) -DDEMO
PERFORMANCE_FLAGS = $(DEMO_FLAGS) -DPERFORMANCE_FLAG

###############################################################################
# Build Directories
###############################################################################
BUILD_DIR       = build
DEBUG_DIR       = $(BUILD_DIR)/debug
RELEASE_DIR     = $(BUILD_DIR)/release
TEST_DIR        = $(BUILD_DIR)/tests
DEMO_DIR        = $(BUILD_DIR)/demo
PERFORMANCE_DIR = $(BUILD_DIR)/performace_test

###############################################################################
# Source files (libraries)
###############################################################################
LIB_SRCS =configurations/config/config.c \
configurations/config_manager.c \
code/process_input/process_input.c \
code/activation_functions/activation_functions.c \
code/cost_functions/cost_functions.c \
code/gradient_descent/gradient_descent.c \
code/helper_functions/helper_functions.c \
code/feed_forward/feed_forward.c \
code/neurons/neurons.c \
code/save_state/save_state.c \
code/structs/structs.c
# Note: We do NOT include 'code/main/main.c' here; we’ll treat it specially below.

# Object files
LIB_OBJS = $(patsubst %.c,%.o,$(LIB_SRCS))

###############################################################################
# Main entry point (only for normal builds)
###############################################################################
MAIN_SRC = code/main/main.c
MAIN_OBJ = $(patsubst %.c,%.o,$(MAIN_SRC))

###############################################################################
# Default binary name
###############################################################################
BIN_NAME = neural_network

###############################################################################
# Default target (release build)
###############################################################################
all: release

###############################################################################
# Release Build
###############################################################################
release: CFLAGS += $(RELEASE_FLAGS)
release: TARGET = $(RELEASE_DIR)/$(BIN_NAME)
release: RELEASE_SRC = $(filter-out code/helper_functions/helper_functions.c, $(LIB_SRCS))
release:
	@mkdir -p $(RELEASE_DIR)
	@$(CC) $(CFLAGS) -fsanitize=address -o $(TARGET) $(RELEASE_SRC) $(MAIN_SRC)
	@echo "Run it for release with:\n./$(TARGET) <file name>"


###############################################################################
# Debug Build
###############################################################################
debug: CFLAGS += $(DEBUG_FLAGS)
debug: TARGET = $(DEBUG_DIR)/$(BIN_NAME)
debug:
	@mkdir -p $(DEBUG_DIR)
	@$(CC) $(CFLAGS) -fsanitize=address -o $(TARGET) $(LIB_SRCS) $(MAIN_SRC)
	@echo "Run it for debugging with:\n./$(TARGET)"

###############################################################################
# Demo Build
###############################################################################
demo: CFLAGS += $(DEMO_FLAGS)
demo: TARGET = $(DEMO_DIR)/$(BIN_NAME)
demo:
	@mkdir -p $(DEMO_DIR)
	@$(CC) $(CFLAGS) -fsanitize=address -o $(TARGET) $(LIB_SRCS) $(MAIN_SRC)
	@echo "Run it for demo with:\n./$(TARGET)"


###############################################################################
# Performance test
###############################################################################
performance_test: CFLAGS += $(PERFORMANCE_FLAGS)
performance_test: TARGET = $(PERFORMANCE_DIR)/$(BIN_NAME)
performance_test: RELEASE_SRC = $(filter-out code/helper_functions/helper_functions.c, $(LIB_SRCS))
performance_test:
	@mkdir -p $(PERFORMANCE_DIR)
	@$(CC) $(CFLAGS) -fsanitize=address -o $(TARGET) $(RELEASE_SRC) $(MAIN_SRC)
	@echo "Run it for performance test with:\n./$(TARGET)"



###############################################################################
# Test Build
#
#   Usage:
#       make test xyz
#
#   This calls the pattern rule "test_xyz" which:
#       1) Excludes main.c and the normal config.c
#       2) Includes test_xyz.c (which has a main())
#       3) Adds xyz.c from the library (since it holds the tested functions)
#       4) Uses test_config.c instead of config.c
###############################################################################
test:
	@echo "Usage: make test_<component>, e.g. 'make test_xyz'"


# Pattern rule: make test_xyz
# This will create a test binary in build/tests/xyz/test_xyz
# Add the TEST_FLAGS to the CFLAGS
test_%: CFLAGS += $(TEST_FLAGS)
test_%:
	@echo "Building test for '$*'"
	@# Create only the top-level test directory
	@mkdir -p $(TEST_DIR)
	@# Produce the binary as build/tests/xyz (no subfolder)
	@$(CC) $(CFLAGS) -fsanitize=address -o $(TEST_DIR)/$* \
		$(filter-out code/main/main.c configurations/config/config.c, $(LIB_SRCS)) \
		code/$*/test/test_$*.c \
		configurations/test_config/test_config.c
	@echo "Run it for testing with:\n./$(TEST_DIR)/$*"
	@echo "\nFor memory leaks, run it with:\n./$(TEST_DIR)/$* &leaks $(pgrep $(TEST_DIR)/$*)\n"

###############################################################################
# Compile library source files to object files
###############################################################################
%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

###############################################################################
# Clean Build Artifacts
###############################################################################
clean:
	rm -rf $(BUILD_DIR)
