// Application layer protocol implementation
#include "link_layer.h"
#include "application_layer.h"
#include <stdio.h>

void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{
    // TODO

// res = read(buf, 1)
// if (res > 0) leu senão não leu
    printf("Receive %s\n", role);
    LinkLayer layer;
    layer.serialPort = serialPort;
    llopen(layer);
    unsigned char trg;
    llread(&trg);
}
