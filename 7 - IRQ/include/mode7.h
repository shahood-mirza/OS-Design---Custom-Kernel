#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <size_t.h>
#include <stdint.h>

extern size_t strlen ( const uint8_t* str );

extern void DisplayChar ( uint8_t c );
extern void ScrollUp();
extern void DisplayString ( uint8_t *str );
extern void ClearScreen (const uint8_t c);
extern void SetColor (const uint8_t c);
extern uint8_t GetColor ();
extern void GotoXY (uint8_t x, uint8_t y);
extern void MoveCursor();
extern void DisplayInteger(uint32_t n);
extern uint8_t GetWidth();
extern uint8_t GetHeight();
extern uint8_t GetXPos();
extern uint8_t GetYPos();

#endif
