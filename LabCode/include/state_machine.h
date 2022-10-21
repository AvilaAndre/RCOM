/**
 * State machine that handles SET and UA messages.
 * Return:
 * 1 -> Finds a correct message.
 * 0 -> Has not (yet) found a correct message. 
*/
int startVerifyState(unsigned char val, int fd, LinkLayerRole role);

void resetDataStateMachine();

/**
 * State machine that handles information frames.
 * Return 
 * -1 if something wrong happened
 * 0 if no action is needed.
 * 1 if the last flag was obtained
 * 2 if a byte to add to the packet was obtained.
 * 3 if a byte that signals stuffing is received.
*/
int dataStateMachine(unsigned char val, int fd, LinkLayerRole role);