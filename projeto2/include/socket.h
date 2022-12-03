#pragma once

int readSocket(int sockfd, int printMSG);

int writeToSocket(int sockfd, char msg[], int size);

int connectToSocket(char serverAddress[], int port);

int disconnectFromSocket(int sockfd);