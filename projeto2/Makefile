# Makefile to build the project
# NOTE: This file must not be changed.

# Parameters
CC = gcc
CFLAGS = -Wall

SRC = src/
INCLUDE = include/
BIN = bin/

# ftp://[<user>:<password>@]<host>/<url-path>
FTP_FEUP = ftp://anonymous:qualquer-password@ftp.up.pt/

# Targets
.PHONY: all
all: $(BIN)/main

$(BIN)/main: main.c $(SRC)/*.c
	$(CC) $(CFLAGS) -o $@ $^ -I$(INCLUDE)
 
.PHONY: clean
clean:
	rm -f $(BIN)/main

.PHONY: run_feup
run_feup:
	make clean && make && $(BIN)/main $(FTP_FEUP)

# make run arg=ftp://ftp.fe.up.pt
.PHONY: run
run:
	make clean && make && $(BIN)/main $(arg)
