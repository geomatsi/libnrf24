#
# LIBNRF24
#
# Variables
#   - CROSS_COMPILE: path/prefix of cross-toolchain
#   - TARGET: target platform identifier to add to library name
#   - PLT_FLAGS: platform-specific compile flags, e.g. -mcpu=...
#   - CFG_FLAGS: linbrf24 configuration switches
#     -- TODO
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

prod: $(LIBNAME)_$(TARGET).a

# TESTS

TEST_DIR = tests

CPP_UTEST_LIBS = -lCppUTest -lCppUTestExt

CMDS_TEST_OBJS = \
	$(TEST_DIR)/main_test.o \
	$(TEST_DIR)/spi_tests.o \
	$(TEST_DIR)/spi_mock.o \
	$(SRC_DIR)/rf24_cmds.o \
	$(SRC_DIR)/rf24.o

cmds_tests: $(CMDS_TEST_OBJS)
	$(CXX) -o cmds_tests $(CMDS_TEST_OBJS) $(CPP_UTEST_LIBS)

run_cmds_tests: cmds_tests
	./cmds_tests -c

CORE_TEST_OBJS = \
	$(TEST_DIR)/main_test.o \
	$(TEST_DIR)/core_tests.o \
	$(TEST_DIR)/uc_tests.o \
	$(TEST_DIR)/cmd_mock.o \
	$(SRC_DIR)/rf24.o

core_tests: $(CORE_TEST_OBJS)
	$(CXX) -o core_tests $(CORE_TEST_OBJS) $(CPP_UTEST_LIBS)

run_core_tests: core_tests
	./core_tests -c


run_tests: run_cmds_tests run_core_tests

#

clean: clean_prod clean_test

clean_prod:
	rm -f $(SRC_DIR)/*.o
	rm -f $(LIBNAME)_$(TARGET).a

clean_test:
	rm -f $(TEST_DIR)/*.o
	rm -f cmds_tests
	rm -f core_tests

.PHONY: clean
