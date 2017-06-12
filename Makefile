#
# LIBNRF24
#
# Variables
#   - CROSS_COMPILE: path/prefix of cross-toolchain
#   - TARGET: target platform identifier to add to library name
#   - PLT_FLAGS: platform-specific compile flags, e.g. -mcpu=...
#   - CFG_FLAGS: linbrf24 configuration switches
#     -- SPI_SINGLE_BYTE -  build with support for single byte SPI xfers
#     -- SPI_MULTI_BYTE - build with support for multi-byte SPI xfers
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

CFG_FLAGS ?= -DSPI_SINGLE_BYTE
#CFG_FLAGS ?= -DSPI_MULTI_BYTE
#CFG_FLAGS ?= -DSPI_SINGLE_BYTE -DSPI_MULTI_BYTE

#

SRCS_DIR = src
PROD_INC = include
TEST_INC = tests/include

CPPFLAGS += -I$(PROD_INC) -I$(TEST_INC)
CPPFLAGS += -Wall $(PLT_FLAGS) $(CFG_FLAGS)

ARFLAGS	= rcs

#

OBJS = \
	$(SRCS_DIR)/rf24.o \
	$(SRCS_DIR)/rf24_sb_cmds.o \
	$(SRCS_DIR)/rf24_mb_cmds.o \

$(LIBNAME)_$(TARGET).a: $(OBJS)
	$(AR) $(ARFLAGS) "$@" $(OBJS)

#

prod: $(LIBNAME)_$(TARGET).a

PREFIX ?= /usr/local

install: prod
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	mkdir -p $(DESTDIR)$(PREFIX)/include
	cp $(LIBNAME)_$(TARGET).a $(DESTDIR)$(PREFIX)/lib/$(LIBNAME).a
	cp $(PROD_INC)/nRF24L01.h $(DESTDIR)$(PREFIX)/include/
	cp $(PROD_INC)/RF24.h $(DESTDIR)$(PREFIX)/include/

uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/lib/$(LIBNAME).a
	rm -rf $(DESTDIR)$(PREFIX)/include/nRF24L01.h
	rm -rf $(DESTDIR)$(PREFIX)/include/RF24.h

#

clean:
	rm -f $(SRCS_DIR)/*.o
	rm -f $(LIBNAME)_$(TARGET).a

.PHONY: uninstall
.PHONY: install
.PHONY: clean
.PHONY: prod
