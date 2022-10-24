#define BUF_SIZE 256

#define TRUE 1
#define FALSE 0

#define BUF_SIZE 256


#define FLAG 0x7E
#define A 0x03 //address for the emitter commands
#define A_RCV 0x01 //address for the receiver commands
#define C_SET 0x03 //set up
#define C_UA 0x07 //unnumbered acknowledgement
#define C_DISC 0x0B //disconnect
#define C_RR0 0x05 //receiver ready / positive acknowledgement
#define C_RR1 0x85 //receiver ready / positive acknowledgement
#define C_REJ0 0x01 //reject / negative acknowledgment
#define C_REJ1 0x81 //reject / negative acknowledgment

#define BCC(a, c) (a^c)

//Information Frame

#define C_ZERO 0x00
#define C_ONE 0x40

//Packets

#define C_DATA 0x01 // data packet
#define C_START 0x02 // control packet
#define C_END 0x03 // control packet
#define T_SIZE 0x00 // control packet
#define T_NAME 0x01 // control packet

#define PACKET_MAX_SIZE 256

//Stuffing

#define MSG_FLAG 0x7E
#define MSG_ESCAPE_BYTE 0x7D
#define MSG_FLAG_STUFFING_BYTE 0x5E
#define MSG_ESCAPE_STUFFING_BYTE 0x5D 