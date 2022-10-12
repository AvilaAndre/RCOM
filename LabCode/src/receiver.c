#include <unistd.h>
#include "macros.h"
#include "link_layer.h"
#include "state_machine.h"
#include "receiver.h"

unsigned char buf[BUF_SIZE] = {0};

int receiverStart(int fd) { //TODO: this might not be correct. What happens after sending the UA?

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