#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "link_layer.h"
#include "macros.h"
#include "sender.h"
#include "receiver.h"
#include <time.h>

// MISC
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

#define BUF_SIZE 256


int fd;

struct termios oldtio;
struct termios newtio;
clock_t start;

LinkLayer connectionInfo;


////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{
    start = clock();
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

    connectionInfo = connectionParameters;
    
    return 1;
}

////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int ca = 0;

int llwrite(const unsigned char *buf, int bufSize)
{
    //Create Information Frame
    unsigned char frame[2*PACKET_MAX_SIZE + 6] = {0};
    
    int frameSize = buildInformationFrame(&frame, buf, bufSize, ca);

    printf("\n>");
    for (int i = 0; i < frameSize; i++) printf("%02x", frame[i]);
    printf("\n");
    printf("\n%d\n", frameSize);

    if (senderInformationSend(frame, frameSize, connectionInfo.nRetransmissions, connectionInfo.timeout) == 0) return -1;


    if (frameSize < 0) return -1; // A error occurred during frame building.
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{
    int num_tries, ret;

    //Exits when it receives a DISC
    ret = receiverRead(fd);
    if(ret != 0)
    {
        fprintf(stderr, "Receiver: Timed out in reading!\n");
        return -1;
    }

    for (num_tries = 0; num_tries < 3; num_tries++) {
        // Reply with DISC, confirming connection ending
        /*ret = writeData(fd, C_DATA, C_DISC);
        if(ret != 0) {
            continue;
        }
        */

        // Waits for Emitter connection ending, UA
        ret = readSupervisionMessage(fd);
        if(ret != 0)
        {
            continue;
        }
        return 0;
        
    }

    return 0;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int showStatistics)
{
    if(showStatistics)
    {
        double cpu_time_used = ((double) (clock() - start)) / CLOCKS_PER_SEC * 1000; //in miliseconds
        printf("The application took %f miliseconds to execute \n", cpu_time_used);
    }

    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
        perror("tcsetattr");
        return -1;
    }

    close(fd);

    return 1;
}
