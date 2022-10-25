/**
 * State machine that handles SET and UA messages.
 * Return:
 * 1 -> Finds a correct message.
 * 0 -> Has not (yet) found a correct message. 
*/
int startVerifyState(unsigned char val, int fd, LinkLayerRole role);


/**
 * Resets the data state machine to its default values.
*/
void resetDataStateMachine();

/**
 * State machine that handles information frames.
 * Return:
 * -1 if something wrong happened
 * 0 if no action is needed.
 * 1 if the last flag was obtained
 * 2 if a byte to add to the packet was obtained.
 * 3 if a byte that signals stuffing is received.
 * 4 received disconnect command.
 * 5 received duplicated frame.
*/
int dataStateMachine(unsigned char val, int fd, unsigned int ca);


/**
 * Handles receiver response.
 * Return:
 * -1 -> Wrong response
 * 0 -> Response not yet received
 * 1 -> Right response
*/
int dataAnswerState(unsigned char val, int fd, int ca);


/**
 * State machine that handles the messages in the llclose section.
 * Return:
 * 0 -> Command not found yet
 * 1 -> Emitter disconnect command received
 * 2 -> Receiver disconnect command received.
 * 3 -> Emitter UA command received
*/
int closeState(unsigned char val, int fd);