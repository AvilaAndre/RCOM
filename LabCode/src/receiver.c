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

int receiverRead(int fd)
{
    unsigned char byte;
    int timeOutCounter = 0;
    const int MAX_TRIES = 3;

    while(1)
    {
        ret = read(fd,&byte,1);

        //if no byte was read or error occurred
        if (ret <= 0) {
            if(time_out_counter < MAX_TRIES) {
                //reset state machine for safety
                //resetStateMachine();??
                time_out_counter++;
                //try again
                continue;
            } else {
                return -1;
            }
        } else {
            time_out_counter = 0;
        }

        dataStateMachine(byte,fd,LlRx);
        //pseudocode because state machine is not done yet
        //case for success
        //alternating between supervision message and info message 0x00 and 0x40
        if(getState() == supervisionMessageOK)
        {
            return 0;
        }

        else if(getState() == infoMessageOK)
        {
            //if RR correct

            //else write correct RR


        }

        else if(getState() == infoError)
        {
            //resetStateMachine
            //write error
        }
    }
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