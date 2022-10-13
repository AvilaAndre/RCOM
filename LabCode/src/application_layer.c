// Application layer protocol implementation
#include <string.h>
#include <stdio.h>
#include "application_layer.h"
#include "link_layer.h"
#include "macros.h"


void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{
    // Opening
    LinkLayer link;

    strncpy(link.serialPort, serialPort, strlen(serialPort) + 1);

    if (strcmp(role, "tx") == 0) {
        link.role = LlTx;
    } else if (strcmp(role, "rx") == 0) {
        link.role = LlRx;
    } else {
        return;
    }

    link.nRetransmissions = nTries;
    link.timeout = timeout;
    link.baudRate = baudRate;

    if (llopen(link) < 0) {
        printf("\nERROR: An error occurred while opening the connection. \n");
        llclose(0);
        return;
    } else {
        printf("\nConnection estabilished. \n");
    }

    if (link.role == LlTx) {
        //TODO: send file
        // I'll start by sending random bits.

        sendControlPacket(filename);


        unsigned char buf[BUF_SIZE];

        for(unsigned int i =0;i<BUF_SIZE;++i){
            buf[i]=i;
        }
        llwrite(buf, BUF_SIZE);
    } else {
        //llread
    }

    if (llclose(0) < 0) { //TODO: Discover what showStatistics means.
        printf("\n ERROR: An error occurred while closing the connection. \n");
        return;
    }
}
