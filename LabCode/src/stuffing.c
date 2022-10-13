#include <stdio.h>
#include <unistd.h>
#include "macros.h"
#include "link_layer.h"
#include "state_machine.h"
#include "alarm.h"
#include "sender.h"
#include "message.h"

int stuffing(unsigned char *data)
{
    unsigned char newdata[BUF_SIZE];
    for(int i = 0; strlen(data); i++)
    {
        if(data[i] == 0x7E) strncat(newdata,(unsigned char)0x7D5E, 2);
        else if(data[i] == 0x7D) strncat(newdata,(unsigned char)0x7D5D, 2);
        else strncat(newdata,data[i],1);
    }

    *data = newdata;
    return 0;
}