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
                savedChars[ptr] = val;
                ptr++;
                return 0;
            }
            break;   
        case 1:
            if (val != 0x7E) {
                state = 2;
                savedChars[ptr] = val;
                ptr++;
                return 0;
            }
        case 2:
            savedChars[ptr] = val;
            ptr++;
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
    AWAITING_FLAG,
    AWAITING_ADDRESS,
    AWAITING_CONTROL,
    AWAITING_BCC,
    RECEIVING_PACKET_TYPE,
    RECEIVING_DATA_COUNTER,
    RECEIVING_CONTROL_T,
    RECEIVING_CONTROL_L,
    RECEIVING_DATA_L1,
    RECEIVING_DATA_L2,
    RECEIVING_DATA,
    RECEIVING_CONTROL,
    AWAITING_LAST_BCC,
    AWAITING_LAST_FLAG
} typedef MACHINE_STATE;

MACHINE_STATE dataState = AWAITING_FLAG;

int dataptr = 0;
unsigned char dataSavedChars[PACKET_MAX_SIZE] = {0};
int dataBit = 0;
int pdataptr = 0;
int dataSize = 0;
int packetSize = 0;
int stufFlag = FALSE;
int t2 = FALSE;

int dataStateMachine(unsigned char byte, int fd, LinkLayerRole role) {
    while (TRUE) {
        switch (dataState)
        {
            case START:
                return -1;
            case AWAITING_FLAG:
                if (byte == FLAG) {
                    dataState = AWAITING_ADDRESS;
                    savedChars[dataptr] = byte;
                    return 0;
                }
                return 0;
            case AWAITING_ADDRESS: 
                if (byte != FLAG) {
                    dataptr++;
                    dataState = AWAITING_CONTROL;
                    savedChars[dataptr++] = byte;
                    return 0;
                }
            case AWAITING_CONTROL: 
                dataState = AWAITING_BCC;
                savedChars[dataptr++] = byte;
                return 0;
            case AWAITING_BCC:
                if (byte == BCC(savedChars[2], savedChars[1])) {
                    dataState = RECEIVING_PACKET_TYPE;
                    savedChars[dataptr++] = byte;
                    return 0;
                } else {
                    dataState = AWAITING_FLAG;
                    dataptr = 0;
                    return -1;
                }
            case RECEIVING_PACKET_TYPE:
                if (byte == C_DATA){
                    dataState = RECEIVING_DATA_COUNTER;
                    t2 = TRUE;
                    savedChars[dataptr++] = byte;
                    dataSavedChars[pdataptr++] = byte;
                    return 0;
                }
                if (byte == C_START || byte == C_END){
                    dataState = RECEIVING_CONTROL_T;
                    savedChars[dataptr++] = byte;
                    dataSavedChars[pdataptr++] = byte;
                    return 0;
                }
                else{
                    dataState = AWAITING_FLAG;
                    dataptr = 0;
                    return -1;
                }
            case RECEIVING_CONTROL_T:
                if(byte == T_SIZE || byte == T_NAME){
                    dataState = RECEIVING_CONTROL_L;
                    savedChars[dataptr] = byte;
                    dataSavedChars[dataptr] = byte;
                }
                else{
                    dataState = AWAITING_FLAG;
                    dataptr = 0;
                    return -1;
                }
            case RECEIVING_CONTROL_L: 
                if(byte == 0x7D){
                    stufFlag = TRUE;
                    return 0;
                }
                if(stufFlag = TRUE){
                    if(byte == 0x5E){
                        byte = 0x7E;
                    }
                    else if (byte == 0x5D){
                        byte = 0x7D;
                    }
                    else {
                        dataState = AWAITING_FLAG;
                        dataptr = 0;
                        return -1;
                    }
                    stufFlag = FALSE;
                }
                savedChars[dataptr] = byte;
                dataSavedChars[pdataptr] = byte;
                packetSize = byte;
                dataSize = byte;
                dataState = RECEIVING_DATA;
                return 0;
            case RECEIVING_DATA_COUNTER:
                if(byte == 0x7D){
                    stufFlag = TRUE;
                    return 0;
                }
                if(stufFlag = TRUE){
                    if(byte == 0x5E){
                        byte = 0x7E;
                    }
                    else if (byte == 0x5D){
                        byte = 0x7D;
                    }
                    else {
                        dataState = AWAITING_FLAG;
                        dataptr = 0;
                        return -1;
                    }
                    stufFlag = FALSE;
                }
                dataState = RECEIVING_DATA_L1;
                savedChars[dataptr++] = byte;
                dataSavedChars[pdataptr++] = byte;
                return 0;
            case RECEIVING_DATA_L1:
                if(byte == 0x7D){
                    stufFlag = TRUE;
                    return 0;
                }
                if(stufFlag = TRUE){
                    if(byte == 0x5E){
                        byte = 0x7E;
                    }
                    else if (byte == 0x5D){
                        byte = 0x7D;
                    }
                    else {
                        dataState = AWAITING_FLAG;
                        dataptr = 0;
                        return -1;
                    }
                    stufFlag = FALSE;
                }
                dataState = RECEIVING_DATA_L2;
                savedChars[dataptr++] = byte;
                dataSavedChars[pdataptr++] = byte;
                return 0;
            case RECEIVING_DATA_L2:
                if(byte == 0x7D){
                    stufFlag = TRUE;
                    return 0;
                }
                if(stufFlag = TRUE){
                    if(byte == 0x5E){
                        byte = 0x7E;
                    }
                    else if (byte == 0x5D){
                        byte = 0x7D;
                    }
                    else {
                        dataState = AWAITING_FLAG;
                        dataptr = 0;
                        return -1;
                    }
                    stufFlag = FALSE;
                }
                dataState = RECEIVING_DATA;
                savedChars[dataptr++] = byte;
                dataSavedChars[pdataptr++] = byte;
                dataSize = 256*savedChars[6] + savedChars[7];
                packetSize = dataSize;
                return 0;
            case RECEIVING_DATA:
                if (dataSize >= 0){
                    if(byte == 0x7D){
                        stufFlag = TRUE;
                        return 0;
                    }
                    if(stufFlag = TRUE){
                        if(byte == 0x5E){
                            byte = 0x7E;
                        }
                        else if (byte == 0x5D){
                            byte = 0x7D;
                        }
                        else {
                            dataState = AWAITING_FLAG;
                            dataptr = 0;
                            return -1;
                        }
                        stufFlag = FALSE;
                    }
                    savedChars[dataptr++] = byte;
                    dataSavedChars[pdataptr++] = byte;
                    if(dataSize == 0){
                        if(t2 == TRUE){
                        dataState = AWAITING_LAST_BCC;
                        }
                        else {
                            dataState = RECEIVING_CONTROL_T;
                            t2 = TRUE;
                        }
                    }
                    dataSize--;
                    return 0;
                }
            case AWAITING_LAST_BCC:
                if(byte == 0x7D){
                    stufFlag = TRUE;
                    return 0;
                }
                if(stufFlag = TRUE){
                    if(byte == 0x5E){
                        byte = 0x7E;
                    }
                    else if (byte == 0x5D){
                        byte = 0x7D;
                    }
                    else {
                        dataState = AWAITING_FLAG;
                        dataptr = 0;
                        return -1;
                    }
                    stufFlag = FALSE;
                }
                unsigned char bcc2 = 0x00;
                for(int i = 0; i < packetSize; i++){
                    if(i == 1){
                        bcc2 = dataSavedChars[i-1]^dataSavedChars[i];
                        i++;
                    }
                    else if (i > 1){
                        bcc2 = bcc2^dataSavedChars[i];
                    }
                }
                if(byte == bcc2) {
                    dataState = AWAITING_LAST_FLAG;
                    savedChars[dataptr++] = byte;
                    dataSavedChars[pdataptr++] = byte;
                    return 0;
                }
                else {
                    dataState = AWAITING_FLAG;
                    dataptr = 0;
                    return -1;
                }
            case AWAITING_LAST_FLAG:
                if (byte == FLAG) {
                    dataState = AWAITING_ADDRESS;
                    savedChars[dataptr] = byte;
                    return 1;
                }
                else {
                    dataState = AWAITING_FLAG;
                    dataptr = 0;
                    return -1;
                }
        }
    }
} 