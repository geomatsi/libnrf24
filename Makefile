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

all: prod

#

SRCS_DIR = src
PROD_INC = include
TEST_INC = tests/include

CPPFLAGS += -I$(PROD_INC) -I$(TEST_INC)
CPPFLAGS += -Wall $(PLT_FLAGS) $(CFG_FLAGS)

ARFLAGS	= rcs

# LIBRARY

SB_OBJS = \
	$(SRCS_DIR)/rf24.o \
	$(SRCS_DIR)/rf24_sb_cmds.o \

$(LIBNAME)_$(TARGET).a: $(SB_OBJS)
	$(AR) $(ARFLAGS) "$@" $(SB_OBJS)

MB_OBJS = \
	$(SRCS_DIR)/rf24.o \
	$(SRCS_DIR)/rf24_mb_cmds.o \

$(LIBNAME)_mb_$(TARGET).a: $(MB_OBJS)
	$(AR) $(ARFLAGS) "$@" $(MB_OBJS)

prod: $(LIBNAME)_$(TARGET).a $(LIBNAME)_mb_$(TARGET).a

#

clean:
	rm -f $(SRCS_DIR)/*.o
	rm -f $(LIBNAME)_$(TARGET).a
	rm -f $(LIBNAME)_mb_$(TARGET).a

.PHONY: clean
