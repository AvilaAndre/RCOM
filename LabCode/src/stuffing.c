#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "macros.h"
#include "stuffing.h"

int stuffing(unsigned char *data, int dataSize)
{    
    unsigned char newdata[PACKET_MAX_SIZE*2+2];
    //stores the last position in where newdata was updated
    int newDataPosition = 0;
    for(int i = 0; i < dataSize; i++)
    {
        if(data[i] == MSG_FLAG) 
        {
            newdata[newDataPosition++] = MSG_ESCAPE_BYTE;
            newdata[newDataPosition++] = MSG_FLAG_STUFFING_BYTE;
        }
        else if(data[i] == MSG_ESCAPE_BYTE) 
        {
            newdata[newDataPosition++] = MSG_ESCAPE_BYTE;
            newdata[newDataPosition++] = MSG_ESCAPE_STUFFING_BYTE;
           
        }       
        else newdata[newDataPosition++] = data[i];
    } 
    for(int i = 0; i < newDataPosition; i++)
    data[i] = newdata[i];
    return newDataPosition;
}