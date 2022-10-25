#include <unistd.h>
#include "macros.h"
#include "link_layer.h"
#include "state_machine.h"
#include "receiver.h"
#include "alarm.h"

unsigned char buf[BUF_SIZE] = {0};

int receiverStart(int fd) {
    while (TRUE) {
        int bytes_ = read(fd, buf, 1);
        if (buf != 0 && bytes_ > -1) {
            int ans = startVerifyState(buf[0], fd, LlRx);
            if (ans == 1) {
                return 1;
            }
        }
        
    }
    return 0;
}

int sendSupervisionFrame(int fd, int type, int ca) {
    unsigned char MSG[5] = {FLAG, A, C_SET, A^C_SET, FLAG};

    if (type == 0) {
        if (ca == 0) 
            MSG[2] = C_REJ0;
        else
            MSG[2] = C_REJ1;
    } else {
        if (ca == 0) 
            MSG[2] = C_RR0;
        else
            MSG[2] = C_RR1;
    }
    MSG[3] = BCC(MSG[1], MSG[2]);

    int bytes = write(fd, MSG, 5);
    if (type == 0) {
        printf("REJ%d flag sent, %d bytes written\n", ca, bytes);
    } else {
        printf("RR%d flag sent, %d bytes written\n", ca, bytes);
    }
    return bytes;
}


int rcvSendDisc(fd) {
    unsigned char MSG[5] = {FLAG, A_RCV, C_DISC, A_RCV^C_DISC, FLAG};

    int bytes = write(fd, MSG, 5);
    printf("\nReceiver DISC flag sent, %d bytes written\n", bytes);
    return bytes;
}

int rcvAwaitDisc(int fd) { 
    unsigned char buf[BUF_SIZE] = {0};

    int bytes_ = read(fd, buf, 1);

    if (buf != 0 && bytes_ > -1) {
        int ans = closeState(buf[0], fd);
        if (ans == 1) {
            return 1;
        }
    }

    return 0;
}

int rcvAwaitUA(int fd) {
    unsigned char buf[BUF_SIZE] = {0};

    int bytes_ = read(fd, buf, 1);

    if (buf != 0 && bytes_ > -1) {
        int ans = closeState(buf[0], fd);
        if (ans == 3) {
            return 1;
        }
    }

    return 0;
}


int rcvNRetransmissions = 0;

int receiverDisconnect(int newNRetransmissions, int timeout, int fd) {

    while(TRUE) {
        int ans = rcvAwaitDisc(fd);
        if (ans) {
            break;
        }
    }

    rcvNRetransmissions = newNRetransmissions;
    while (TRUE) {

        if (!alarmEnabled) {
            if (rcvNRetransmissions == 0) return 0;
            rcvSendDisc(fd);
            rcvNRetransmissions--;
            startAlarm(timeout);
        }

        if (rcvAwaitUA(fd) == 1)  {
            printf("\nDISC Received, sending UA\n");
            senderSendDiscUA(fd);
            return 1;
        }
    }
    return 0;
}