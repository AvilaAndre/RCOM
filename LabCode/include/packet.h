/**
 * Transforms the filename and fileSize into a control packet, returning the control packet through reference and the amount of bytes on the packet.
*/
unsigned int getControlPacket(unsigned char *filename, int fileSize, int start, unsigned char *packet);

/**
 * Receives fileData as a parameter as well as the counter, creating a data packet that is returned through reference and returning the amount of bytes on the packet.
*/
unsigned int getDataPacket(unsigned char *fileData, unsigned int dataSize, unsigned int counter);

/**
 * Handles a packet, returning the type of packet and returning the packet through reference as well as the size of the packet.
 * Return:
 * 3 -> End of file received, packet is now the filename and size is the size of the file.
 * 2 -> Start of file to receive, packet is now the filename and size is the size of the file.
 * 1 -> Data packet received, packet is now the data transferred and size is the size of the data packet.
*/
unsigned int handlePacket(unsigned char *packet, unsigned int *size);