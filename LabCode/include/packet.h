/**
 * Transforms the filename and fileSize into a control packet, returning the control packet through reference and the amount of bytes on the packet.
*/
unsigned int getControlPacket(unsigned char *filename, int fileSize, int start, unsigned char *packet);

/**
 * Receives fileData as a parameter as well as the counter, creating a data packet that is returned through reference and returning the amount of bytes on the packet.
*/
unsigned int getDataPacket(unsigned char *fileData, unsigned int dataSize, unsigned int counter, unsigned char *packet);