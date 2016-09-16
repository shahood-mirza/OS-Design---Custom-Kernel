#include <port.h>

uint8_t inport (uint16_t _port)
{
    uint8_t rv;
    __asm__ __volatile__ ("in %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}


void outport(uint16_t port, uint8_t data)
{
    __asm__ __volatile__ ("out %1, %0" : : "dN" (port), "a" (data));
}

