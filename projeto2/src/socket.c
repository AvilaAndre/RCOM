#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"

#include <string.h>


int readSocket(int sockfd) {
    char newBuf[1];
    char lastChar = 0x00;


    int codeFound = FALSE;
    int newLine = FALSE;
    int code = 0;

    while(1) {
        // printf("start\n");

        int readBytes = read(sockfd, newBuf, 1);

        if (!codeFound) {
            if (newBuf[0] >= 0x30 && newBuf[0] <= 0x39) {
                code = code *10;
                code += newBuf[0] - 0x30;
            } else if (newBuf[0] == 0x2D) {
                newLine = TRUE;
                codeFound = TRUE;
            } else {
                codeFound = TRUE;
            }
        }

        // printf("end\n");
        if (readBytes <= 0) break;

        newBuf[readBytes] = '\0';

        printf("%c", newBuf[0]);

        //printf("response b %02x last %02x\n", newBuf[0], lastChar);
        if (newBuf[0] == 0x0A && lastChar == 0x0D && codeFound) {
            //printf("code: %d\n", code);
            codeFound = FALSE;
            if (!newLine)
                break;
            newLine = FALSE;
            code = 0;
        }
        lastChar = newBuf[0];
    }

    return code;
}


int connectToSocket(char serverAddress[], int port) {
    int sockfd;
    struct sockaddr_in server_addr;


    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(serverAddress);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);        /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }
	printf("socket opened\n");
    /*connect to the server*/
    if (connect(sockfd,
                (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect()");
        return -1;
    }
	printf("socket connected\n");

    return sockfd;
}

int disconnectFromSocket(int sockfd) {
    if (close(sockfd)<0) {
        perror("close()");
        exit(-1);
    }
    printf("socket disconnected\n");
}