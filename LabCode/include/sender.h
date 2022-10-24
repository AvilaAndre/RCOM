/**
 * Sends the SET message.
 * Returns:
 * -> The number of bytes written.
*/
int sendSET();

int senderReceive();

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


int sendFrame(unsigned char frameToSend[], int frameToSendSize);

/**
 * Receive receiver response
 * Return:
 * 0 -> Not received
 * 1 -> Not the right response
 * 2 -> The right response
*/
int senderInformationReceive(int ca);


int senderInformationSend(unsigned char frameToSend[], int frameToSendSize, int newNRetransmissions, int timeout, int ca);

/**
 * Receives the disconnect command.
 * Return:
 * 0 -> if not received.
 * 1 -> received
*/
int awaitDisc(); 


/**
 * Sends a DISC to the receiver.
 * Retuns the number of bytes written.
*/
int sendDisc();

/**
 * Sends a DISC to the receiver and awaits for response.
 * Return:
 * 1 -> Success
 * 0 -> Failure
*/
int senderDisconnect(int newNRetransmissions, int timeout, int fd);