// Alarm example
//
// Modified by: Eduardo Nuno Almeida [enalmeida@fe.up.pt]

#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#define FALSE 0
#define TRUE 1

int alarmEnabled = FALSE;
int alarmCount = 0;


void killAlarm() {
    alarmEnabled = FALSE;
    alarmCount = 0;
}

// Alarm function handler
void alarmHandler(int signal)
{
    alarmEnabled = FALSE;
    alarmCount++;
    printf("Alarm #%d\n", alarmCount);
}

int startAlarm(unsigned int duration)
{
    // Set alarm function handler
    (void)signal(SIGALRM, alarmHandler);

    if (alarmEnabled == FALSE)
    {
        alarm(duration); // Set alarm to be triggered in 3s
        alarmEnabled = TRUE;
    }

    //printf("Ending program\n");

    return 0;
}
