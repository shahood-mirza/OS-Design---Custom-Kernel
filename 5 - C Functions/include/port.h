#ifndef __PORT_H__
#define	__PORT_H__

#include <stdint.h>

uint8_t inportb (uint16_t);

void outportb (uint16_t , uint8_t);

void outport (uint16_t, uint8_t);

#endif

