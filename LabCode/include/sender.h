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


/**
 * Build an information frame with the packet received.
 * Return:
 * -1 -> Error building the frame
 * >0 -> Size of frame
*/
int buildInformationFrame(unsigned char *frame, unsigned char packet[], int packetSize,  unsigned int ca);