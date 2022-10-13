#include <stdio.h>
#include <unistd.h>
#include "macros.h"
#include "link_layer.h"
#include "state_machine.h"
#include "alarm.h"
#include "sender.h"
#include "message.h"

unsigned char *getControlPacket(unsigned char *filename, int start) {

    unsigned char packet[BUF_SIZE] = {0};
    int filenameSize = strlen(filename);
    
    int size = 0;
    if (start == 1) {
        packet[0] = C_START;
        packet[1] = T_NAME;
        packet[2] = filenameSize;
        size = 3;
        for (int i = 0; i < strlen(filename); i++) {
            packet[size] = filename[i];
            size++;
        }
    }


    return 1;
}