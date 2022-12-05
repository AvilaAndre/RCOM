#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define FALSE 0
#define TRUE 1
#define MAX_STRING_LENGTH 256
typedef char url_data[MAX_STRING_LENGTH];

typedef struct URL {
	url_data user; 
	url_data password; 
	url_data host; 
	url_data path; 
	url_data filename;
	int port; 
} url;

enum parseState {read_FTP, read_USR, read_PASS, read_HOST, read_PATH};

void resetURL(url* url);
void parseURL(url* url, const char *input_url);
void getPass(url *url);