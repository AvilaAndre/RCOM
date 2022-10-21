#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "macros.h"
#include "link_layer.h"
#include "state_machine.h"



int state = 0;
unsigned char savedChars[BUF_SIZE] = {};
int ptr = 0;

//TODO: return -1 on error
int startVerifyState(unsigned char val, int fd, LinkLayerRole role) {  
    while (TRUE) {
        switch (state)
        {
        case 0:
            if (val == 0x7E) {
                printf("log > Received flag \n");
                state = 1;
                savedChars[ptr++] = val;
                return 0;
            }
            break;   
        case 1:
            if (val != 0x7E) {
                state = 2;
                savedChars[ptr++] = val;
                return 0;
            }
        case 2:
            savedChars[ptr++] = val;
            if (val == 0x7E){
                state = 3;
            } else {
                return 0;
            }
        case 3:
            if (savedChars[3] == BCC(savedChars[1], savedChars[2]) && ptr > 4) {
                state = 4;
            } else {
                state = 0;
                printf("log > Bad input, restarting... \n");
                ptr = 0;
                return 0;
            } 
        case 4:
            state = 0;
            ptr = 0;
            if (role == LlRx) {
                if (savedChars[2] == 0x03) {
                    savedChars[2] = 0x07;
                    savedChars[3] = BCC(savedChars[1],savedChars[2]);
                    savedChars[4] = 0x7E;
                    printf("log > Sending UA \n");
                    write(fd, savedChars, BUF_SIZE);
                    return 1;
                }
            } else if (role == LlTx) {
                if (savedChars[2] == 0x07) {
                    printf("log > Deactivating alarm. \n");
                    return 1;
                }
            }
            printf("log > Wrong C \n");
            return -1;
        default:
            break;
        }
        return 0;
    }
}


//TODO: merge with function on top ↑ ↑


enum mst {
    START,
    FLAG_RECEIVED,
    ADDRESS_RECEIVED,
    CONTROL_RECEIVED,
    BCC1_RECEIVED,
    RECEIVING_PACKET,
    AWAITING_LAST_FLAG
} typedef MACHINE_STATE;

MACHINE_STATE dataState = START;

int dataptr = 0;
unsigned char dataSavedChars[PACKET_MAX_SIZE] = {0};
int dataBit = 0;
int pdataptr = 0;
int dataSize = 0;
int packetSize = 0;
int stufFlag = FALSE;
int t2 = FALSE;

/**
 * Return -1 se nao der certo
 * 0 nao for para fazer nada
 * 1 recebi ultima flag
 * 2 byte do packet
 * 3 byte de stuffing
*/

int dataStateMachine(unsigned char byte, int fd, LinkLayerRole role) {
    while (TRUE) {
        switch (dataState)
        {
            case START:
                if (byte == 0x7E) {
                    dataState = FLAG_RECEIVED;
                    dataSavedChars[dataptr++] = byte;
                    return 0;
                }
                break;   
            case FLAG_RECEIVED:
                if (byte == A) {
                    dataState = ADDRESS_RECEIVED;
                    dataSavedChars[dataptr++] = byte;
                    return 0;
                }
                break;
            case ADDRESS_RECEIVED:
                if (byte != FLAG) {
                    dataState = CONTROL_RECEIVED;
                    savedChars[dataptr++] = byte;
                    return 0;
                }
                break;
            case CONTROL_RECEIVED: 
                if (byte == BCC(savedChars[2], savedChars[1])) {
                    dataState = RECEIVING_PACKET;
                    savedChars[dataptr++] = byte;
                    return 0;
                } else {
                    dataState = BCC1_RECEIVED;
                    dataptr = 0;
                    return -1;
                }
                break;
            case RECEIVING_PACKET:
                if (byte == 0x7D) {
                    return 3;
                } else if (byte == 0x7E) {
                    dataState = START;
                    dataptr = 0;
                    return 1;
                } else {
                    return 2;
                }
                break;
        }
    }
} 