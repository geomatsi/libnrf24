#
# LIBNRF24
#
# Variables
#   - CROSS_COMPILE: path/prefix of cross-toolchain
#   - TARGET: target platform identifier to add to library name
#   - PLT_FLAGS: platform-specific compile flags, e.g. -mcpu=...
#   - CFG_FLAGS: linbrf24 configuration switches
#     -- LIB_RF24_SIZEOPT
#     -- LIB_RF24_SWEEP_RPD
#

LIBNAME = libnrf24
TARGET ?= x86

#

#CROSS_COMPILE ?= arm-none-eabi-

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
CXX = $(CROSS_COMPILE)g++

#

all: run_tests

#

PROD_HDR = include
TEST_HDR = tests/include

CPPFLAGS += -I$(PROD_HDR) -I$(TEST_HDR)
CPPFLAGS += -Wall $(PLT_FLAGS) $(CFG_FLAGS)

# LIBRARY

SRC_DIR = src

OBJS = \
	$(SRC_DIR)/rf24.o \
	$(SRC_DIR)/rf24_cmds.o \

ARFLAGS	= rcs

$(LIBNAME)_$(TARGET).a: $(OBJS)
	$(AR) $(ARFLAGS) "$@" $(OBJS)

build_prod: $(LIBNAME)_$(TARGET).a

# TESTS

TEST_DIR = tests

TEST_OBJS = \
	$(TEST_DIR)/main_test.o \
	$(TEST_DIR)/basic_tests.o \
	$(TEST_DIR)/cmd_tests.o \
	$(TEST_DIR)/mock.o

TEST_LIBS = -lCppUTest -lCppUTestExt

build_tests: build_prod $(TEST_OBJS)
	$(CXX) -o run_tests $(TEST_OBJS) $(TEST_LIBS) $(LIBNAME)_$(TARGET).a

run_tests: build_tests
	./run_tests -c

#

clean: clean_prod clean_test

clean_prod:
	rm -f $(SRC_DIR)/*.o
	rm -f $(LIBNAME)_$(TARGET).a

clean_test:
	rm -f $(TEST_DIR)/*.o
	rm -f run_tests

.PHONY: clean
