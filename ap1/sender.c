// Write to serial port in non-canonical mode
//
// Modified by: Eduardo Nuno Almeida [enalmeida@fe.up.pt]

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include "alarm.c"

// Baudrate settings are defined in <asm/termbits.h>, which is
// included by <termios.h>
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

#define BUF_SIZE 256

volatile int STOP = FALSE;

int state = 0;
unsigned char saved_chars[BUF_SIZE] = {};
int ptr = 0;


int verify_state(unsigned char val, int fd) {  
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
                printf("log > Input, restarting... \n");
                ptr = 0;
                return 0;
            } 
        case 4:
            state = 0;
            ptr = 0;
            if (saved_chars[2] == 0x07) {
                printf("log > Deactivating alarm. \n");
                killAlarm();
                return 1;
            }
            printf("log > Wrong C \n");
            return -1;
        default:
            break;
        }
        return 0;
    }
}

int fd;

unsigned char buf[BUF_SIZE +1] = {0};


int sendSET() {
    buf[0] = 0x7E; // FLAG
    buf[1] = 0x03; // A
    buf[2] = 0x03; // C
    buf[3] = 0x00; // BCC
    buf[4] = 0x7E; // FLAG

    int bytes = write(fd, buf, 5);
    printf("SET flag sent, %d bytes written\n", bytes);
    return bytes;
}


int main(int argc, char *argv[])
{
    // Program usage: Uses either COM1 or COM2
    const char *serialPortName = argv[1];

    if (argc < 2)
    {
        printf("Incorrect program usage\n"
               "Usage: %s <SerialPort>\n"
               "Example: %s /dev/ttyS1\n",
               argv[0],
               argv[0]);
        exit(1);
    }

    // Open serial port device for reading and writing, and not as controlling tty
    // because we don't want to get killed if linenoise sends CTRL-C.
    fd = open(serialPortName, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (fd < 0)
    {
        perror(serialPortName);
        exit(-1);
    }

    struct termios oldtio;
    struct termios newtio;

    // Save current port settingsS
    if (tcgetattr(fd, &oldtio) == -1)
    {
        perror("tcgetattr");
        exit(-1);
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

    // VTIME e VMIN should be changed in order to protect with a
    // timeout the reception of the following character(s)

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred to
    // by fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    sendSET();

    // Wait until all bytes have been written to the serial port
    sleep(1);

    while (STOP == FALSE)
    {
        if (!alarmEnabled) {
            sendSET(); 
        }
        startAlarm(3);

        // Returns after 5 chars have been input
        int bytes_ = read(fd, buf, 1);
        if (buf != 0 && bytes_ > -1) {
            int ans = verify_state(buf[0], fd);
            if (ans == 1) {
                STOP = TRUE;
            }
        }
    }

    // Restore the old port settings
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    close(fd);

    return 0;
}
