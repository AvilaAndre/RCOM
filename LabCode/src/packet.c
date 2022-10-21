#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "macros.h"
#include "packet.h"

unsigned int getControlPacket(unsigned char *filename, int fileSize, int start, unsigned char packet[]) {

    unsigned int filenameSize = strlen(filename);

    unsigned int bytesForFilenameSize = filenameSize/256;
    if (filenameSize - (bytesForFilenameSize * 256) > 0) {
        bytesForFilenameSize++;
    }
    
    unsigned char hexaSize[BUF_SIZE] = {0};
    
    sprintf(hexaSize, "%X", fileSize);

    unsigned int bytesForFileSize = strlen(hexaSize);

    if (bytesForFileSize % 2) bytesForFileSize++;

    bytesForFileSize = bytesForFileSize/2;

    unsigned int bytesToSend = 3+ bytesForFilenameSize + filenameSize + bytesForFileSize;

    int size = 0;
    int sizePos = 0;
    if (start == TRUE) {
        packet[size] = C_START;
    } else {
        packet[size] = C_END;
    }
    packet[size + 1] = T_NAME;
    packet[size + 2] = strlen(filename);
    size += 2 + bytesForFilenameSize;
    for (int i = 0; i < strlen(filename); i++) {
        packet[3 + i] = filename[i];
    }
    strncat(packet, filename, filenameSize);
    size += filenameSize;
    sizePos = size;
    packet[size] = 0x02;
    packet[size + 1] = bytesForFileSize;
    size += 2; 
    for (int k = bytesForFileSize-1; k >= 0; k--) {
        packet[size + bytesForFileSize-k-1] = (unsigned char) (fileSize >> (8*k));
    }
    size += bytesForFileSize;
    packet[sizePos] = T_SIZE;

    return size;
}


unsigned int getDataPacket(unsigned char *fileData, unsigned int dataSize, unsigned int counter) {
    unsigned char packet[PACKET_MAX_SIZE] = {0};

    int l1 = dataSize/256;
    int l2 = dataSize%256;

    packet[0] = C_DATA;
    packet[1] = counter % 256;
    packet[2] = l1;
    packet[3] = l2;
    for (int k = 0; k < dataSize; k++) {
        packet[4+k] = fileData[k];
    }
    for (int k = 0; k < (dataSize+4); k++) {
        fileData[k] = packet[k];
    }

    return dataSize + 4;
}

unsigned int handlePacket(unsigned char *packet, unsigned int *size) {
    unsigned int counter = 0;
    unsigned int nameSize = 0;
    unsigned int sizeSize = 0;

    switch (packet[0])
    {
    case C_START:
        if (packet[1*8] == T_SIZE) {
            sizeSize = packet[2*8];
            unsigned int newSize = 0;
            int i = 0;
            for (i = 0; i < sizeSize; i++) {
                newSize |= packet[(3+i)*8] << 8*(sizeSize-i-1); 
            }
            (*size) = newSize;
            i += 3;
            if (packet[i*8] != T_NAME) {
                printf("ERROR: not name \n");
                return 0;
            }
            nameSize = packet[(i+1) * 8];

            for (int j = 0; j < nameSize; j++) {
                packet[j] = packet[(i+2+j)*8];
            }
            packet[nameSize] = '\0';

        } else if (packet[1*8] == T_NAME) {
            unsigned int newSize = 0;
            nameSize = packet[2*8];

            sizeSize = packet[(4+nameSize)*8];

            for (int i = 0; i < sizeSize; i++) {
                newSize |= packet[(5+nameSize+i)*8] << 8*(sizeSize-i-1); 
            }
            (*size) = newSize;

            for (int j = 0; j < nameSize; j++) {
                packet[j] = packet[(3+j)*8];
            }
            packet[nameSize] = '\0';
        }
        return 2;
    case C_END:
        if (packet[1*8] == T_SIZE) { // Avila: By my code this situation will never happen tbh.
            sizeSize = packet[2*8];
            unsigned int newSize = 0;
            int i = 0;
            for (i = 0; i < sizeSize; i++) {
                newSize |= packet[(3+i)*8] << 8*(sizeSize-i-1); 
            }
            (*size) = newSize;
            i += 3;
            if (packet[i*8] != T_NAME) {
                printf("ERROR: not name \n");
                return 0;
            }
            nameSize = packet[(i+1) * 8];

            for (int j = 0; j < nameSize; j++) {
                packet[j] = packet[(i+2+j)*8];
            }
            packet[nameSize] = '\0';

        } else if (packet[1*8] == T_NAME) {
            unsigned int newSize = 0;
            nameSize = packet[2*8];

            sizeSize = packet[(4+nameSize)*8];

            for (int i = 0; i < sizeSize; i++) {
                newSize |= packet[(5+nameSize+i)*8] << 8*(sizeSize-i-1); 
            }
            (*size) = newSize;

            for (int j = 0; j < nameSize; j++) {
                packet[j] = packet[(3+j)*8];
            }
            packet[nameSize] = '\0';
        }
        return 3;
    case C_DATA:
        counter = packet[1*8];
        (*size) = packet[2*8] * 256 + packet[3*8];
        printf("packet read:"); //TODO: delete this debug;
        for (int j = 0; j < (*size); j++) {
            packet[j] = packet[j*8 + 4*8];
            printf("|%02x", packet[j]);
        }
        printf("\nsize %d \n", (*size));
        return 1;
    
    default:
        return 0;
    }
}