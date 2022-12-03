#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"
#include "socket.h"

#include <string.h>


int readSocket(int sockfd, int *extraReturn) {
    char newBuf[1];
    char lastChar = 0x00;
    // When in passive mode
    int dataAddress[6] = {0,0,0,0,0,0};
    int dataAddressPointer = 0;
    int reading = FALSE;
    int numberRead = 0;


    int codeFound = FALSE;
    int newLine = FALSE;
    int code = 0;

    while(1) {
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
        } else {
            switch (code)
            {
            case FTP_PASV:
                if(reading) {
                    if (newBuf[0] == 0x29) { // 0x29 => ')'
                        reading = FALSE;
                        dataAddress[dataAddressPointer++] = numberRead;

                        char address[256] = {0};
                        int port = dataAddress[4] * 256 + dataAddress[5];

                        sprintf(address, "%d.%d.%d.%d", dataAddress[0], dataAddress[1], dataAddress[2], dataAddress[3]);
                        // printf("\n%s.%d \n", address, port); //DEBUG:


                        //connect to new socket and return it's fd
                        printf("\nConnecting to data socket\n\n");
                        (*extraReturn) = connectToSocket(address, port);
                    } else if (newBuf[0] == 0x2c) { // 0x2c => ','
                        dataAddress[dataAddressPointer++] = numberRead;
                        numberRead = 0;
                    } else if (newBuf[0] >= 0x30 && newBuf[0] <= 0x39) {
                        numberRead = numberRead * 10;
                        numberRead += newBuf[0] - 0x30;
                    }
                } else {
                    if (newBuf[0] == 0x28) reading = TRUE; // 0x28 => '('
                }
                break;
            case FTP_FILE_RCV:
                if(reading) {
                    if (newBuf[0] == 0x29) { // 0x29 => ')'
                        printf("\nReceiving file will have %d bytes\n\n", numberRead);
                        (*extraReturn) = numberRead;
                    } else if (newBuf[0] >= 0x30 && newBuf[0] <= 0x39) {
                        numberRead = numberRead * 10;
                        numberRead += newBuf[0] - 0x30;
                    }
                } else {
                    if (newBuf[0] == 0x28) reading = TRUE; // 0x28 => '('
                }
                break;
            default:
                break;
            }

        }

        if (readBytes <= 0) break;

        newBuf[readBytes] = '\0';

        if (newBuf[0] == 0x0A && lastChar == 0x0D && codeFound) {
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
        return -1;
    }
    printf("socket disconnected\n");
    return 0;
}

int writeToSocket(int sockfd, char msg[], int size) {
    char *buf = malloc(size+2);

    for (int i = 0; i< size; i++) {
        buf[i] = msg[i];
    }
    if (msg[size-1] == '\0') {
        buf[size-1] = '\r';
        buf[size] = '\n';
        buf[size+1] = '\0';
    } else {
        buf[size] = '\r';
        buf[size+1] = '\n';
        buf[size+2] = '\0';
    }

    // printf("writing: %s", buf); // DEBUG:
    int bytes = write(sockfd, buf, strlen(buf));
    if (bytes <= 0) {
        perror("write()");
        return -1;
    }

    return bytes;
}

int readDataSocketToFile(int sockfd, int *filefd, int fileSize) {
    char newBuf[256];
    char lastChar = 0x00;
    
    while(fileSize) {
        int readBytes = read(sockfd, newBuf, 256);

        if (readBytes <= 0) break;

        if (newBuf[0] == 0x0A && lastChar == 0x0D) {
            break;
        }
        lastChar = newBuf[0];

        for (int i = 0; i < readBytes; i++) {
            fputc(newBuf[i], filefd);
        }

        fileSize -= readBytes;
    }

    return 0;
}