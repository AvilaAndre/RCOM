#pragma once

#define FTP_PASV 227

int readSocket(int sockfd, int *extraReturn);

int writeToSocket(int sockfd, char msg[], int size);

int connectToSocket(char serverAddress[], int port);

int disconnectFromSocket(int sockfd);