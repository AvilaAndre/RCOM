/**
 * Transforms the filename and fileSize into a control packet.
*/
unsigned char *getControlPacket(unsigned char *filename, int fileSize, int start);

/**
 * Receives fileData as a parameter as well as the counter, creating a data packet.
*/
unsigned char *getDataPacket(unsigned char *fileData, unsigned int dataSize, unsigned int counter);