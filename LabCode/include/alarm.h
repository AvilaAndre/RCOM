#include "macros.h"

extern int alarmEnabled;
extern int alarmCount;


void killAlarm();

void alarmHandler(int signal);

int startAlarm(unsigned int duration);