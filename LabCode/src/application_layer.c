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

        struct stat file;
        stat(filename, &file);

        int file_fd;

        if ((file_fd = open(filename, O_RDONLY)) < 0){
            perror("Error opening file.");
            llclose(0);
            return;
        }

        unsigned int bytesToSend;
        unsigned char buf[PACKET_MAX_SIZE] = {0};

        bytesToSend = getControlPacket(filename, file.st_size, TRUE, buf);

        llwrite(buf, bytesToSend);

        unsigned int counter = 0;
        int count = 0;
        while ((bytesToSend = read(file_fd, buf, PACKET_MAX_SIZE-4)) > 0) {
            count += bytesToSend;
            bytesToSend = getDataPacket(&buf, bytesToSend, counter); // TODO: counter logic

            if (llwrite(buf, bytesToSend) < 0) {
                printf("Failed to send information frame\n");
                llclose(0);
                return -1;
            }

            printf("Sending... %d%% sent. \n",(int) (((double)count / (double)file.st_size) *100));
            //debugEND
        }



    } else {
        int *fileToWrite = -1;

        unsigned char *buf[PACKET_MAX_SIZE] = {0};
        llread(&buf);
        unsigned int packetSize = 0;

        switch (handlePacket(&buf, &packetSize)) {
            case 0:
                perror("Error handling packet.");
                llclose(0);
                return;
            case 1:
                if (fileToWrite == -1) {
                    perror("No file to write initialized.");
                    return;
                }
                // Writing to file
                for (int j = 0; j < packetSize; j++) {
                    fputc(buf[j], fileToWrite);
                }
                break;
            case 2:
                printf("Start ControlPacket: %s %d %d\n", buf, strlen(buf), packetSize);
                fileToWrite =  fopen(filename, "w+");
                break;
            case 3:
                printf("End ControlPacket: %s %d %d\n", buf, strlen(buf), packetSize);
                break;
        }
    }

    if (llclose(0) < 0) { //TODO: Discover what showStatistics means.
        printf("\n ERROR: An error occurred while closing the connection. \n");
        return;
    }
}
