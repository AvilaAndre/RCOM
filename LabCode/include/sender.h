/**
 * Sends the SET message.
 * Returns:
 * -> The number of bytes written.
*/
int sendSET();

int senderReceive();

void senderAlarmHandler(int signal);

/**
 * Sends the SET message and waits for the response every -timeout- seconds for a maximum of -newNRetransmissions- times.
 * 
 * Return:
 * 0 -> if it fails
 * 1 -> if it suceeds
*/
int senderStart(int newfd, int newNRetransmissions, int timeout);