#ifndef PIT_H
#define PIT_H
#include <stdint.h>
#include <registers.h>

//sets up the PITHandler assumed Kernel Mode 32-bit
void initPITHandler();

//using IRQ0
void PITHandler(struct isrregs *r);

//draw clock with hours, minutes and seconds in the top-right corner
void drawClock(int, int, int);

#endif
