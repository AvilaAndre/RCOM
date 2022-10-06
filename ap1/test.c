#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


int main() {
    unsigned char buf[2] = {0};
    unsigned char word = 0x7E; 
    unsigned char ADDRESS = 0x74;

    buf[0] = 0x7E;
    buf[1] = 0x70;
             //0x0A
    // unsigned char* CONTROL = str_slice(word, 16, 24);
    // unsigned char* BCC = str_slice(word, 8, 32);


    printf("% result \n", word^ADDRESS);
    printf("%s result \n", buf);
    return 0;
}