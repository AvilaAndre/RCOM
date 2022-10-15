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

    unsigned int bytes_to_send = 3+ bytesForFilenameSize + filenameSize + bytesForFileSize;

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
    for (int i = 0; i < bytesForFilenameSize; i++) {
        packet[3 + i] = filename[i];
        printf("%02x %c \n", filename[i], filename[i]);
    }
    printf("size: %d \n", filenameSize);
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

    for (int i = 0; i < size; i++) {
        //printf("%d -> %02X %c \n", i, packet[i], packet[i]);
    }

    return size;
}


unsigned int getDataPacket(unsigned char *fileData, unsigned int dataSize, unsigned int counter, unsigned char *packet) {

    int l1 = dataSize/256;
    int l2 = dataSize%256;

    packet[0] = C_DATA;
    packet[1] = counter % 256;
    packet[2] = l1;
    packet[3] = l2;
    for (int k = 0; k < dataSize; k++) {
        packet[4+k] = fileData[k];
    }

    return dataSize + 4;
}

unsigned int handlePacket(unsigned char *packet, unsigned int *size) {
    printf("handlePacket called \n"); //TODO: remove this line
    unsigned int counter = 0;
    unsigned int nameSize = 0;
    unsigned int sizeSize = 0;

    switch (packet[0])
    {
    case C_START:
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
        counter = packet[1];
        size = packet[2] * 256 + packet[3];
        packet += 4;
        return 1;
    
    default:
        return 0;
    }
}