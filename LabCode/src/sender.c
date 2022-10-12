#include <stdio.h>
#include <unistd.h>
#include "macros.h"
#include "link_layer.h"
#include "state_machine.h"
#include "alarm.h"
#include "sender.h"

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