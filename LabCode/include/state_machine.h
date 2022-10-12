/**
 * State machine that handles SET and UA messages.
 * Return:
 * 1 -> Finds a correct message.
 * 0 -> Has not (yet) found a correct message. 
*/
int startVerifyState(unsigned char val, int fd, LinkLayerRole role);