#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include "macros.h"
#include "alarm.h"

int alarmEnabled = FALSE;
int alarmCount = 0;


void killAlarm() {
    alarmEnabled = FALSE;
    alarmCount = 0;
}


void alarmHandler(int signal) {
    alarmEnabled = FALSE;
    alarmCount++;
    printf("Alarm #%d\n", alarmCount);
}



int startAlarm(unsigned int duration)
{
    (void)signal(SIGALRM, alarmHandler);

    if (alarmEnabled == FALSE)
    {
        alarm(duration);
        alarmEnabled = TRUE;
    }

    return 0;
}
