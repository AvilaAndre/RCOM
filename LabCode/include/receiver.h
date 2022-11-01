
/**
 * Receives the SET message and responds with the UA message.
 *
 * Return:
 * 0 -> if it fails
 * 1 -> if it suceeds
 */
int receiverStart(int fd);

/**
 * type 0 is REJ and 1 is RR
 */
int sendSupervisionFrame(int fd, int type, int ca);

/**
 * Sends the disconnect command.
 */
int rcvSendDisc(int fd);

/**
 * Awaits the disconnect command.
 */
int rcvAwaitDisc(int fd);

/**
 * Awaits the unknown acknowledgement command.
 */
int rcvAwaitUA(int fd);

/**
 * Sends the disconnect command and awaits for an UA
 */
int receiverDisconnect(int newNRetransmissions, int timeout, int fd);