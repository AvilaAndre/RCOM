#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "macros.h"
#include "link_layer.h"
#include "state_machine.h"



int state = 0;
unsigned char saved_chars[BUF_SIZE] = {};
int ptr = 0;


int startVerifyState(unsigned char val, int fd, LinkLayerRole role) {  
    while (TRUE) {
        switch (state)
        {
        case 0:
            if (val == 0x7E) {
                printf("log > Received flag \n");
                state = 1;
                saved_chars[ptr] = val;
                ptr++;
                return 0;
            }
            break;   
        case 1:
            if (val != 0x7E) {
                state = 2;
                saved_chars[ptr] = val;
                ptr++;
                return 0;
            }
        case 2:
            saved_chars[ptr] = val;
            ptr++;
            if (val == 0x7E){
                state = 3;
            } else {
                return 0;
            }
        case 3:
            if (saved_chars[3] == (saved_chars[1]^saved_chars[2]) && ptr > 4) {
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
                if (saved_chars[2] == 0x03) {
                    saved_chars[2] = 0x07;
                    saved_chars[3] = saved_chars[1]^saved_chars[2];
                    saved_chars[4] = 0x7E;
                    printf("log > Sending UA \n");
                    write(fd, saved_chars, BUF_SIZE);
                    return 1;
                }
            } else if (role == LlTx) {
                if (saved_chars[2] == 0x07) {
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