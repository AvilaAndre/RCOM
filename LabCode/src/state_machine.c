#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "macros.h"
#include "link_layer.h"
#include "state_machine.h"



int state = 0;
unsigned char savedChars[BUF_SIZE] = {};
int ptr = 0;

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
                printf("log > Protocol error. \n");
                ptr = 0;
                return 0;
            } 
        case 4:
            state = 0;
            ptr = 0;
            if (role == LlRx) {
                if (savedChars[2] == C_SET) {
                    savedChars[2] = C_UA;
                    savedChars[3] = BCC(savedChars[1],savedChars[2]);
                    savedChars[4] = 0x7E;
                    printf("log > Sending UA \n");
                    write(fd, savedChars, BUF_SIZE);
                    state = 0;
                    ptr = 0;
                    return 1;
                }
            } else if (role == LlTx) {
                if (savedChars[2] == C_UA) {
                    printf("log > Deactivating alarm. \n");
                    state = 0;
                    ptr = 0;
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

enum mst {
    START,
    FLAG_RECEIVED,
    ADDRESS_RECEIVED,
    CONTROL_RECEIVED,
    RECEIVING_PACKET
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


void resetDataStateMachine() {
    dataState = START;
    dataptr = 0;
}


int dataStateMachine(unsigned char byte, int fd, unsigned int ca) {
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
                    if (!((byte == C_ZERO && ca == 0) || (byte == C_ONE && ca == 1))) {
                        printf("log > Duplicated frame. \n");
                        resetDataStateMachine();
                        return 5;
                    }
                    return 0;
                }
                break;
            case CONTROL_RECEIVED: 
                if (byte == BCC(savedChars[2], savedChars[1])) {
                    dataState = RECEIVING_PACKET;
                    savedChars[dataptr++] = byte;
                    return 0;
                } else if (byte == FLAG){
                    dataState = FLAG_RECEIVED;
                    dataptr = 0;
                    return 0;
                } else {
                    printf("log > Protocol error. \n");
                    dataptr = 0;
                    resetDataStateMachine();
                    return 0;
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
        return 0;
    }
} 


int resState = 0;
unsigned char resSavedChars[BUF_SIZE] = {};
int resptr = 0;

int dataAnswerState(unsigned char val, int fd, int ca) {  
    while (TRUE) {
        switch (resState)
        {
        case 0:
            if (val == 0x7E) {
                resState = 1;
                resSavedChars[resptr++] = val;
                return 0;
            }
            break;   
        case 1:
            if (val != 0x7E) {
                resState = 2;
                resSavedChars[resptr++] = val;
                return 0;
            }
        case 2:
            resSavedChars[resptr++] = val;
            if (val == 0x7E){
                resState = 3;
            } else {
                return 0;
            }
        case 3:
            if (resSavedChars[3] == BCC(resSavedChars[1], resSavedChars[2]) && resptr > 4) {
                state = 4;
            } else {
                resState = 0;
                printf("log > Protocol error. \n");
                resptr = 0;
                return 0;
            } 
        case 4:
            resState = 0;
            resptr = 0;
            if (resSavedChars[2] == C_RR0) {
                printf("RR0 received. \n");
                resState = 0;
                resptr = 0;
                if (ca == 0) return 1; else return -1;
            } else if (resSavedChars[2] == C_RR1) {
                printf("RR1 received. \n");
                resState = 0;
                resptr = 0;
                if (ca == 1) return 1; else return -1;
            } else if (resSavedChars[2] == C_REJ0) {
                printf("REJ0 received. \n");
                resState = 0;
                resptr = 0;
                return -1;
            } else if (resSavedChars[2] == C_REJ1) {
                printf("REJ1 received. \n");
                resState = 0;
                resptr = 0;
                return -1;
            }
            printf("log > Wrong C \n");
            return -1;
        default:
            break;
        }
        return 0;
    }
}

int endState = 0;
unsigned char endSavedChars[BUF_SIZE] = {};
int endptr = 0;

int closeState(unsigned char byte, int fd) {
    while (TRUE) {
        switch (endState)
        {
            case START:
                if (byte == FLAG) {
                    endState = FLAG_RECEIVED;
                    endSavedChars[endptr++] = byte;
                    return 0;
                }
                break;   
            case FLAG_RECEIVED:
                if (byte == A_RCV) {
                    endState = ADDRESS_RECEIVED;
                    endSavedChars[endptr++] = byte;
                    return 0;
                } else if (byte == A) {
                    endState = ADDRESS_RECEIVED;
                    endSavedChars[endptr++] = byte;
                    return 0;
                }
                break;
            case ADDRESS_RECEIVED:
                if (byte != FLAG) {
                    endState = CONTROL_RECEIVED;
                    endSavedChars[endptr++] = byte;
                    return 0;
                }
                break;
            case CONTROL_RECEIVED: 
                if (byte == BCC(endSavedChars[2], endSavedChars[1])) {
                    endState = RECEIVING_PACKET;
                    endSavedChars[endptr++] = byte;
                    return 0;
                } else {
                    printf("log > Procotol error. \n");
                    endState = FLAG_RECEIVED;
                    endptr = 0;
                    return -1;
                }
                break;
            case RECEIVING_PACKET:
                if (byte == 0x7E) {
                    endState = START;
                    endptr = 0;
                    if (endSavedChars[2] == C_UA) {
                        return 3;
                    } else if (endSavedChars[2] == C_DISC && endSavedChars[1] == A_RCV) {
                        return 2;
                    } else if (endSavedChars[2] == C_DISC && endSavedChars[1] == A) {
                        return 1;

                    }
                    return 0;
                }
                break;
        }
    }
}