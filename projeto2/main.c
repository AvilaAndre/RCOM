#include "include/socket.h"
#include "include/utils.h"


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
    printf(">>%s", argv[1]);
    char hostname[20] = {0};
    printf("\ngetHostname: %d\n", getHostnameString(argv[1], hostname));
    printf("hostname: %s\n", hostname);
    
    getIP(hostname);
}
