#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "macros.h"
#include "packet.h"

unsigned char *getControlPacket(unsigned char *filename, int fileSize, int start) {

    int filenameSize = strlen(filename);

    int bytesForFilenameSize = filenameSize/256;
    if (filenameSize - (bytesForFilenameSize * 256) > 0) {
        bytesForFilenameSize++;
    }
    
    unsigned char hexaSize[BUF_SIZE] = {0};
    
    sprintf(hexaSize, "%X", fileSize);

    int bytesForFileSize = strlen(hexaSize);

    if (bytesForFileSize % 2) bytesForFileSize++;

    bytesForFileSize = bytesForFileSize/2;

    unsigned char *packet = malloc(2 + bytesForFilenameSize + filenameSize + 1 + bytesForFileSize);

    int size = 0;
    int sizePos = 0;
    if (start == 1) {
        packet[size] = C_START;
        packet[size + 1] = T_NAME;
        packet[size + 2] = strlen(filename);
        size += 2 + bytesForFilenameSize;
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
    }

    return packet;
}


unsigned char *getDataPacket(unsigned char *fileData, unsigned int dataSize, unsigned int counter) {

    int l1 = dataSize/256;
    int l2 = dataSize%256;

    

    unsigned char *packet = malloc(4 + dataSize);

    packet[0] = C_DATA;
    packet[1] = counter % 256;
    packet[2] = l1;
    packet[3] = l2;
    for (int k = 0; k < dataSize; k++) {
        packet[4+k] = fileData[k];
    }

    return packet;
}