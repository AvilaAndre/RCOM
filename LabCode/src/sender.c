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

void senderAlarmHandler(int signal) {
    alarmEnabled = FALSE;
    alarmCount++;
    sendSET();
    nRetransmissions--;
    printf("Alarm #%d\n", alarmCount);
}

int senderStart(int newfd, int newNRetransmissions, int timeout) {
    thisfd = newfd;
    nRetransmissions = newNRetransmissions;
    sendSET();
    while (nRetransmissions > 0) {
        startAlarm(timeout, senderAlarmHandler);

        if (senderReceive(thisfd) == 1) return 1; 
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
    //Find BCC2
    unsigned char bcc2 = 0x00;
    for(int i = 0; i < packetSize; i++){
        if(i == 1){
            bcc2 = packet[i-1]^packet[i];
            i++;
        }
        else if (i > 1){
            bcc2 = bcc2^packet[i];
        }
        frame[frameSize++] = packet[i];
    }
    frame[frameSize++] = bcc2;
    frame[frameSize++] = FLAG;

    printf("\n");
    for (int i = 0; i < frameSize; i++) printf("%02x", frame[i]);
    printf("\n");
    printf("\n%d\n", frameSize);

    return frameSize;
}