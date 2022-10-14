// Application layer protocol implementation
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "application_layer.h"
#include "link_layer.h"
#include "macros.h"
#include "packet.h"

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

        struct stat file;
        stat(filename, &file);
        file.st_size;

        int file_fd;

        if ((file_fd = open(filename, O_RDONLY)) < 0){
            perror("Error opening file.");
            return -1;
        }

        getControlPacket(filename, file.st_size, 1);

        unsigned int counter = 0;
        unsigned char *buf = malloc(128);
        unsigned int bytes_to_send;
        unsigned progress = 0;
        
        while ((bytes_to_send = read(file_fd, buf, PACKET_MAX_SIZE - 4)) > 0) {
            //DEBUG-START: show what 
            // printf("\n");
            // for (int i = 0; i < bytes_to_send; i++) {
            //     printf(" [%d/%d->%02x-%c]", i, bytes_to_send, buf[i], buf[i]);
            // }
            // printf("\n");
            buf = getDataPacket(buf, bytes_to_send, counter);
            bytes_to_send += 4;
            //DEBUG-END
            llwrite(buf, bytes_to_send);
        }



    } else {
        //llread
    }

    if (llclose(0) < 0) { //TODO: Discover what showStatistics means.
        printf("\n ERROR: An error occurred while closing the connection. \n");
        return;
    }
}
