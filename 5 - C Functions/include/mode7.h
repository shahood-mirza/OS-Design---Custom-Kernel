#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <size_t.h>
#include <stdint.h>
#include "port.h"

#define VID_MEMORY	0xB8000
#define	COLS		80
#define	LINES		25

extern size_t strlen ( const uint8_t* str );

extern void DisplayChar ( uint8_t c );
extern void ScrollUp();
extern void DisplayString ( uint8_t *str );
extern void ClearScreen (const size_t c);
extern void SetColor (const size_t c);
extern size_t GetColor ();
extern void GotoXY (uint32_t x, uint32_t y);
extern void MoveCursor();
extern void OutPort(uint16_t port, uint32_t data);
extern void DisplayInteger(uint32_t n);
extern uint32_t GetWidth();
extern uint32_t GetHeight();
extern uint32_t GetXPos();
extern uint32_t GetYPos();

#endif
