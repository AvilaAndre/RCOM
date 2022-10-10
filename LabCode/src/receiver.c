#include "macros.h"

volatile int STOP = 0;

int byt_ptr = 0;

unsigned char buf[BUF_SIZE] = {0};

void receiverStart(int fd) {

    while (STOP == FALSE)
    {
        int bytes_ = read(fd, buf, 1);
        if (buf != 0 && bytes_ > -1) {
            printf("Received %02x \n", buf[0]);
            // /int ans = verify_state(buf[0], fd);
            // if (ans == 1) {
            //     STOP = TRUE;
            // }
        }
        
    }
}