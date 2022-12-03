# Makefile to build the project
# NOTE: This file must not be changed.

# Parameters
CC = gcc
CFLAGS = -Wall

SRC = src/
INCLUDE = include/
BIN = bin/

# ftp://[<user>:<password>@]<host>/<url-path>
FTP_FEUP = ftp://ftp.up.pt/pub/kodi/timestamp.txt
FTP_FEUP2 = ftp://anonymous:qualquer-password@ftp.up.pt/pub/kodi/robots.txt
FTP_KODI1 = ftp://anonymous:qualquer-password@ftp.up.pt/pub/kodi/screenshots/kodi-addons.jpg
FTP_KODI2 = ftp://anonymous:qualquer-password@ftp.up.pt/pub/kodi/screenshots/kodi-videolibrary.jpg
FTP_KODIAPK = ftp://anonymous:qualquer-password@ftp.up.pt/pub/kodi/snapshots/android/arm/kodi-18.0-Leia_alpha2-armeabi-v7a.apk 

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

.PHONY: run_feup2
run_feup2:
	make clean && make && $(BIN)/main $(FTP_FEUP2)

.PHONY: run_kodi1
run_kodi1:
	make clean && make && $(BIN)/main $(FTP_KODI1)

.PHONY: run_kodi2
run_kodi2:
	make clean && make && $(BIN)/main $(FTP_KODI2)

.PHONY: run_kodiapk
run_kodiapk:
	make clean && make && $(BIN)/main $(FTP_KODIAPK)


# make run arg=ftp://ftp.fe.up.pt
.PHONY: run
run:
	make clean && make && $(BIN)/main $(arg)
