#
# LIBNRF24
#
# Variables
#   - CROSS_COMPILE: path/prefix of cross-toolchain
#   - TARGET: target platform identifier to add to library name
#   - PLT_FLAGS: platform-specific compile flags, e.g. -mcpu=...
#   - CFG_FLAGS: libstlinky configuration switches
#     -- LIB_RF24_SIZEOPT
#     -- LIB_RF24_SWEEP_RPD
#

LIBNAME = libnrf24
TARGET ?= stm32f4

#

#CROSS_COMPILE ?= arm-none-eabi-

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
CXX = $(CROSS_COMPILE)g++

#

all: run_tests

#

ARFLAGS	= rcs

SRC_HDR = include
TEST_HDR = tests

CPPFLAGS += -I$(SRC_HDR) -I$(TEST_HDR)
CPPFLAGS += -Wall $(PLT_FLAGS) $(CFG_FLAGS)

# LIBRARY

SRC_DIR = src

OBJS = \
	$(SRC_DIR)/rf24.o \
	$(SRC_DIR)/rf24-sweep.o

#%.o: %.c
#	$(CC) $(CFLAGS) -o $@ -c $<

$(LIBNAME)_$(TARGET).a: $(OBJS)
	$(AR) $(ARFLAGS) "$@" $(OBJS)

prod: $(LIBNAME)_$(TARGET).a

# TESTS

TEST_DIR = tests
TEST_LIBS = -lCppUTest -lCppUTestExt

TEST_OBJS = \
	$(TEST_DIR)/main_test.o \
	$(TEST_DIR)/basic_tests.o \
	$(TEST_DIR)/cmd_tests.o \
	$(TEST_DIR)/stubs.o \
	$(SRC_DIR)/rf24_ops.o \
	$(SRC_DIR)/rf24.o \

build_tests: $(TEST_OBJS)
	$(CXX) -o run_tests $(TEST_OBJS) $(TEST_LIBS)

run_tests: build_tests
	./run_tests -c

#

clean: clean_prod clean_test

clean_prod:
	rm -f $(LIBNAME)_$(TARGET).a
	rm -f $(SRC_DIR)/*.o

clean_test:
	rm -f $(TEST_DIR)/*.o
	rm -f run_tests

.PHONY: clean
