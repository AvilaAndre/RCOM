#include <stdio.h>
#include <unistd.h>
#include "macros.h"
#include "link_layer.h"
#include "state_machine.h"
#include "alarm.h"
#include "sender.h"
#include "stuffing.h"

int thisfd;
int nRetransmissions;


int sendSET() {
    
    unsigned char SET[5] = {FLAG, A, C_SET, A^C_SET, FLAG};

    int bytes = write(thisfd, SET, 5);
    printf("SET flag sent, %d bytes written\n", bytes);
    return bytes;
}

int senderReceive() {

    unsigned char buf[BUF_SIZE] = {0};

    int bytes_ = read(thisfd, buf, 1);
    if (buf != 0 && bytes_ > -1) {
        int ans = startVerifyState(buf[0], thisfd, LlTx);
        if (ans == 1) {
            killAlarm();
            return 1;
        }
    }

    return 0;
}


int senderStart(int newfd, int newNRetransmissions, int timeout) {
    thisfd = newfd;
    nRetransmissions = newNRetransmissions;
    while (nRetransmissions > 0) {


        if (!alarmEnabled) {
            sendSET();
            nRetransmissions--;
            startAlarm(timeout);
        }


        if (senderReceive() == 1) return 1; 
    }
    return 0;
}

int buildInformationFrame(unsigned char *frame, unsigned char packet[], int packetSize,  unsigned int ca) {

    int frameSize = 4;

    frame[0] = FLAG;
    //Set Address
    frame[1] = A;
    //Set Alternating Control Address
    if(ca == 0){
        frame[2] == C_ZERO;
    } else {
        frame[2] == C_ONE;
    }
    //Set BCC1
    frame[3] = frame[1]^frame[2];

    unsigned char newPacket[PACKET_MAX_SIZE*2+2] = {0};
    //Find BCC2
    unsigned char bcc2 = 0x00;
    for(int i = 0; i < packetSize; i++){
        newPacket[i] = packet[i];
        if(i == 1){
            bcc2 = packet[i-1]^packet[i];
        }
        else if (i > 1){
            bcc2 = bcc2^packet[i];
        }
    }

    newPacket[packetSize+1] = bcc2;

    packetSize = stuffing(newPacket, packetSize+1);

    for (int i = 0; i < packetSize; i++)
    {
        frame[frameSize++] = newPacket[i];
    }
    

    frame[frameSize++] = FLAG;

    return frameSize;
}


int sendFrame(unsigned char frameToSend[], int frameToSendSize) { 
    int bytes = write(thisfd, frameToSend, frameToSendSize);
    printf("\n");
    for (int i = 0; i < frameToSendSize; i++) printf("%02x", frameToSend[i]);
    printf("\n");
    printf("Information frame sent, %d bytes written\n", bytes);
    return bytes;
}


int senderInformationReceive() {

    unsigned char buf[BUF_SIZE] = {0};

    int bytes_ = read(thisfd, buf, 1);
    if (buf != 0 && bytes_ > -1) {
        /* int ans = dataAnswerState(buf[0], thisfd, LlTx); //TODO: this
        if (ans == 1) {
            killAlarm();
            return 1;
        }*/
        return 1;
    }

    return 0;
}


int senderInformationSend(unsigned char frameToSend[], int frameToSendSize, int newNRetransmissions, int timeout) {
    nRetransmissions = newNRetransmissions;
    while (nRetransmissions > 0) {

        if (!alarmEnabled) {
            sendFrame(frameToSend, frameToSendSize);
            nRetransmissions--;
            startAlarm(timeout);
        }
        return 1;
        //if (senderInformationReceive() == 1) return 1;  // TODO:  
    }
    return 0;
}