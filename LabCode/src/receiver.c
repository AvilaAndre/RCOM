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

int readSupervisionMessage(int fd) {
 /*   //resetMsgState()
    byte msg_byte;
    ssize_t ret;
    while(getState() != SUP_MSGRECEIVED) {
        // Ensure the message received is a supervision message
        if (getState() == INFO_MSG_RECEIVED || getState() == MSG_ERROR) {
            resetMsgState();
        }

        ret = read(fd, &msg_byte, 1);

        if (ret <= 0) {
            return ret;
        }

        dataStateMachine(byte,fd,LlRx);
    }
    */
    return 0;
}