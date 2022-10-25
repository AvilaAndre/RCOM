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
    while (TRUE) {

        if (!alarmEnabled) {
            if (nRetransmissions == 0) {
                printf("log > Timed out \n");
                return 0;
            }
            sendSET();
            nRetransmissions--;
            startAlarm(timeout);
        }

        if (senderReceive() == 1) return 1; 
    }
    return 0;
}

int buildInformationFrame(unsigned char *frame, unsigned char packet[], int packetSize, unsigned int ca) {

    int frameSize = 4;

    frame[0] = FLAG;
    //Set Address
    frame[1] = A;
    //Set Alternating Control Address
    if(ca == 0){
        frame[2] = C_ZERO;
    } else {
        frame[2] = C_ONE;
    }
    //Set BCC1
    frame[3] = frame[1]^frame[2];

    unsigned char newPacket[PACKET_MAX_SIZE*2+2] = {0};
    //Find BCC2
    unsigned char bcc2 = 0x00;
    for(int i = 0; i < packetSize; i++){
        newPacket[i] = packet[i];
        bcc2 = BCC(bcc2,packet[i]);
    }

    newPacket[packetSize] = bcc2;

    packetSize = stuffing(newPacket, packetSize+1);

    for (int i = 0; i < packetSize; i++)
    {
        frame[frameSize++] = newPacket[i];
    }
    

    frame[frameSize++] = FLAG;

    return frameSize;
}


int sendFrame(unsigned char frameToSend[], int frameToSendSize) { 
    // unsigned char old_byte = frameToSend[2]; //Uncomment to test a faulty packet.
    // int r = rand() % 20;
    // if (r == 13) {
    //     printf("SENDING WRONG INFO FRAME\n");
    //     frameToSend[2] = 0x43;
    // }
    // if (r == 11) {
    //     printf("SENDING DUPLICATE FRAME!\n");
    //     write(thisfd, frameToSend, frameToSendSize);
    // }
    int bytes = write(thisfd, frameToSend, frameToSendSize);
    // frameToSend[2] = old_byte;
    printf("Information frame sent, %d bytes written\n", bytes);
    return bytes;
}


int senderInformationReceive(int ca) {

    unsigned char buf[BUF_SIZE] = {0};

    int bytes_ = read(thisfd, buf, 1);
    if (buf != 0 && bytes_ > -1) {
        int ans = dataAnswerState(buf[0], thisfd, ca);
        if (ans == 1) {
            killAlarm();
            return 1;
        }
    }

    return 0;
}


int senderInformationSend(unsigned char frameToSend[], int frameToSendSize, int newNRetransmissions, int timeout, int ca) {
    nRetransmissions = newNRetransmissions;
    while (TRUE) {

        if (!alarmEnabled) {
            if (nRetransmissions == 0) {
                printf("log > Timed out \n");
                return 0;
            }
            sendFrame(frameToSend, frameToSendSize);
            nRetransmissions--;
            startAlarm(timeout);
        }

        if (senderInformationReceive(ca) == 1)  {
            return 1;
        }
    }
    return 0;
}


int senderSendDisc(fd) {
    unsigned char MSG[5] = {FLAG, A, C_DISC, A^C_DISC, FLAG};

    int bytes = write(fd, MSG, 5);
    printf("\nEmitter DISC flag sent, %d bytes written\n", bytes);
    return bytes;
}

int senderSendDiscUA(int fd) {
    unsigned char MSG[5] = {FLAG, A_RCV, C_UA, A_RCV^C_UA, FLAG};

    int bytes = write(fd, MSG, 5);
    printf("\nEmitter UA flag sent, %d bytes written\n", bytes);
    return bytes;
}

int awaitDisc(fd) {
    unsigned char buf[BUF_SIZE] = {0};
    int bytes_ = read(fd, buf, 1);
    if (buf != 0 && bytes_ > -1) {
        int ans = closeState(buf[0], fd);
        if (ans == 2) {
            killAlarm();
            return 1;
        }
    }

    return 0;
}

int senderDisconnect(int newNRetransmissions, int timeout, int fd) {

    while (TRUE) {

        if (!alarmEnabled) {
            if (nRetransmissions == 0) {
                printf("log > Timed out \n");
                return 0;
            }
            senderSendDisc(fd);
            nRetransmissions--;
            startAlarm(timeout);
        }

        if (awaitDisc(fd) == 1)  {
            printf("\nDISC Received, sending UA\n");
            senderSendDiscUA(fd);
            return 1;
        }
    }
    return 0;
}