#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "URL.h"
#include "utils.h"
#include "socket.h"


int getHostnameString(char hostString[], char hostname[]) {
    int maxSize = strlen(hostString);
    int counting = 0;
    int idx = 0;
    for (int i = 0; i < maxSize; i++) {
        if (counting && hostString[i] == '/') break;
        if (hostString[i] == '/' && hostString[i-1] == '/') {
            counting = 1;
            continue;
        }
        if (counting) hostname[idx++] = hostString[i];
    }
    hostname[idx] = '\0';
    return 1;
}


int main(int argc, char **argv) {
    if (argc != 2) {
		fprintf(stderr, "Wrong number of arguments!");
		return 1;
	}

	struct URL url;
	resetURL(&url);

    parseURL(&url,argv[1]);

    printf("Username: %s\n", url.user);
	printf("Password: %s\n", url.password);
	printf("Host: %s\n", url.host);
	printf("Path: %s\n", url.path);
	printf("Filename: %s\n", url.filename);

    struct hostent *h;
    
    h = getIP(url.host);

    char* ip = inet_ntoa(*((struct in_addr *) h->h_addr_list[0]));

    printf("ip: %s\n", ip);
    printf("port: %d\n", url.port);

    int socketfd;
    if((socketfd = connectToSocket(ip,url.port)) == -1)
    {
		fprintf(stderr, "Unable to connect to socket");
		exit(1);
	}

    readSocket(socketfd, NULL);
    
    int response = 0;

    // Sending username
    char *username = malloc(10 + strlen(url.user));
    username[0] = '\0';

    strcat(username, "user ");
    strcat(username, url.user);

    writeToSocket(socketfd, username, strlen(username));

    response = readSocket(socketfd, NULL);

    if (response != 331) {
        printf("Login failed. Wrong username\n");
        exit(-1);
    }

    // Sending password

    char *password = malloc(10 + strlen(url.password));
    password[0] = '\0';

    strcat(password, "pass ");
    strcat(password, url.password);
    
    writeToSocket(socketfd, password, strlen(password));

    response = readSocket(socketfd, NULL);

    if (response != 230) {
        printf("Login failed. Wrong password\n");
        exit(-1);
    } else {
        printf("\nLogin successful.\n\n");
    }

    // Entering passive mode

    writeToSocket(socketfd, "pasv", 4);

    int datasocket = 0;

    readSocket(socketfd, &datasocket);

    char *retr = malloc(10 + strlen(url.path));
    retr[0] = '\0';

    strcat(retr, "retr ");
    strcat(retr, url.path);

    writeToSocket(socketfd, retr, strlen(retr));

    //Check what the size of the file is

    int fileSize = 0;

    readSocket(socketfd, &fileSize);

    int *fileToWrite = -1;
    

    fileToWrite = fopen(url.filename, "w+");

    readDataSocketToFile(datasocket, fileToWrite, fileSize);

    fclose(fileToWrite);

    printf("File received.\n\n");


    if (disconnectFromSocket(socketfd) == -1) exit(-1);

    return 0;
}
