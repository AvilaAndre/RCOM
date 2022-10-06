// Read from serial port in non-canonical mode
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

// Baudrate settings are defined in <asm/termbits.h>, which is
// included by <termios.h>
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

#define BUF_SIZE 256

volatile int STOP = FALSE;


int state = 0;
unsigned char saved_chars[];


int verify_state(unsigned char val, int fd) {
    printf("VERIFY \n");

    printf("VERIFY VAL: %hhu \n", val);
    printf("VERIFY STATE: %d \n", state);
    
    int ptr = 0;
    switch (state)
    {
    case 0:
        if (val == 0x7E) {
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
        }
        return 0;
    case 3:
        if (saved_chars[3] == (saved_chars[1]^saved_chars[2]) && ptr > 4) {
            state = 4;
        } else {
            state = 0;
            ptr = 0;
        }
        break;  
    case 4:
        state = 0;
        ptr = 0;
        if (saved_chars[2] == 0x03) {
            saved_chars[2] = 0x07;
            int bytes = write(fd, saved_chars, BUF_SIZE);
        }
    default:
        break;
    }
    return 0;
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

    // Open serial port device for reading and writing and not as controlling tty
    // because we don't want to get killed if linenoise sends CTRL-C.
    int fd = open(serialPortName, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror(serialPortName);
        exit(-1);
    }

    struct termios oldtio;
    struct termios newtio;

    // Save current port settings
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

    // Loop for input
    unsigned char buf[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char

    unsigned char ret[5] = "";
    int byt_ptr = 0;

    while (STOP == FALSE)
    {
        // Returns after 5 chars have been input
        while(byt_ptr != 5) {
            read(fd, buf, 1);
            if (buf != 0) {
                ret[byt_ptr] = buf[0];
                //printf(":%s\n", buf);
                byt_ptr++;
                printf("BEFORE VERIFY \n");
                verify_state(buf[0], fd);
            }
        }
        STOP = TRUE;
        ret[byt_ptr] = '\0';
        printf("> Sender: %s \n", ret);
    }

    int bytes = write(fd, ret, BUF_SIZE);
    printf("%d bytes written by the receiver \n", bytes);


    //STATE MACHINE

    // The while() cycle should be changed in order to respect the specifications
    // of the protocol indicated in the Lab guide

    // Restore the old port settings
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    close(fd);

    return 0;
}



// SET=[FLAG,A,C,BCC,FLAG]
// SET = 0x7E | 0x03 | 0x03 | BCC | 0x7E