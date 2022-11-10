# Makefile to build the project
# NOTE: This file must not be changed.

# Parameters
CC = gcc
CFLAGS = -Wall

SRC = src/
INCLUDE = include/
BIN = bin/
CABLE_DIR = cable/

TX_SERIAL_PORT = /dev/ttyS10
RX_SERIAL_PORT = /dev/ttyS11

TX_FILE = penguin.gif
RX_FILE = penguin-received.gif

# Targets
.PHONY: all
all: $(BIN)/main $(BIN)/cable

$(BIN)/main: main.c $(SRC)/*.c
	$(CC) $(CFLAGS) -o $@ $^ -I$(INCLUDE)

$(BIN)/cable: $(CABLE_DIR)/cable.c
	$(CC) $(CFLAGS) -o $@ $^
 
.PHONY: clean
clean:
	rm -f $(BIN)/main
