#pragma once

#define FTP_PASV 227
#define FTP_FILE_RCV 150

int readSocket(int sockfd, int *extraReturn);

int writeToSocket(int sockfd, char msg[], int size);

int connectToSocket(char serverAddress[], int port);

int disconnectFromSocket(int sockfd);

int readDataSocketToFile(int sockfd, int *filefd, int fileSize);