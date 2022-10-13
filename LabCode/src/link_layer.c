#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "link_layer.h"
#include "macros.h"
#include "sender.h"
#include "receiver.h"

// MISC
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

#define BUF_SIZE 256


int fd;

struct termios oldtio;
struct termios newtio;


////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{
    printf("Opening connection %s \n", connectionParameters.serialPort);

    fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (fd < 0)
    {
        perror(connectionParameters.serialPort);
        return -1;
    }

    // Save current port settingsS
    if (tcgetattr(fd, &oldtio) == -1)
    {
        perror("tcgetattr");
        return -1;
    }

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // Set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0; // Inter-character timer unused
    newtio.c_cc[VMIN] = 5;  // Blocking read until 5 chars received
    tcflush(fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        return -1;
    }

    printf("New termios structure set\n");

    if (connectionParameters.role == LlRx) {
        if (!receiverStart(fd)) return -1;
    } else {
        if (!senderStart(fd, connectionParameters.nRetransmissions, connectionParameters.timeout)) return -1;
    }
    
    return 1;
}

////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int ca = 0;

int llwrite(const unsigned char *buf, int bufSize)
{
    //Create Information Frame
    unsigned char frame[BUF_SIZE] = {0};
    //Set Flag
    frame[0] = FLAG;
    //Set Address
    frame[1] = A;
    //Set Alternating Control Address
    if(ca == 0){
        frame[2] == C_ZERO;
    }
    else{
        frame[2] == C_ONE;
    }
    //Set BCC1
    frame[3] = frame[1]^frame[2];
    //Find BCC2
    unsigned char bcc2 = 0x00;
    for(int i = 0; i < bufSize; i++){
        if(bcc2 = 0x00){
            bcc2 = buf[i]^buf[i+1];
            i++;
        }
        else{
            bcc2 = bcc2^buf[i];
        }
    }
    unsigned char tmp[bufSize+1] = buf 

    }
    
    /*
    unsigned char b[len + 1];
    for(int i = 0; i < strlen(a); i++){
        b[i] = a[i];
    }
    */

    /*printf("\n");
    for (int i = 0; i < bufSize; i++) printf("%02x ",buf[i]);
    printf("\n");*/

    return 0;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{
    // TODO

    return 0;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int showStatistics)
{
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
        perror("tcsetattr");
        return -1;
    }

    close(fd);

    return 1;
}
