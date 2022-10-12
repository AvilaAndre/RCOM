#define BUF_SIZE 256

#define TRUE 1
#define FALSE 0

#define BUF_SIZE 256


#define FLAG 0x7E
#define A 0x03 //address
#define C_SET 0x03 //set up
#define C_AU 0x07 //unnumbered acknowledgement
#define C_DISC 0x0B //disconnect
#define C_RR(n) 0x05 | (n>>7) //receiver ready / positive acknowledgement
#define C_REJ(n) 0x01 | (n>>7) //reject / negative acknowledgment