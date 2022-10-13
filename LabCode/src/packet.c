#include <stdio.h>
#include <unistd.h>
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
        packet[size] = 0x02; //T_SIZE
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