#pragma once

int readSocket(int sockfd);

int connectToSocket(char serverAddress[], int port);

int disconnectFromSocket(int sockfd);