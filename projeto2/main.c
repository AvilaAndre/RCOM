#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "include/URL.h"
#include "include/utils.h"
#include "include/socket.h"


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
	printf("Path :%s\n", url.path);
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

    return 0;
		
}
