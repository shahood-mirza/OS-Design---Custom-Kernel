#include <stdarg.h>
#include <mode7.h>

#define VID_MEMORY	0xB8000
#define	COLS		80
#define	LINES		25


static uint32_t _xPos=0, _yPos=0;
static size_t _color=0;

size_t strlen ( const uint8_t* str ) 
{
	size_t	len=0;
	while (str[len++]);
	return len;
}


void DisplayChar(uint8_t c)
{

	if (c == 0)
		return;

	if (c == '\n')
	{
		_yPos++;
		_xPos=0;
		MoveCursor();
		return;
	}

	if (_xPos >= COLS)
	{
		_yPos++;
		_xPos=0;
	}

	uint8_t* p = (uint8_t*)VID_MEMORY + (_xPos++)*2 + _yPos * COLS * 2;
	*p++ = c;
	*p =_color;
	MoveCursor();

}


void DisplayString(uint8_t *str)
{
	if (!str)
		return;

	size_t i;
	for (i=0; i < strlen(str); i++)
		DisplayChar(str[i]);
}


void ClearScreen (const size_t c) 
{

	uint8_t* p = (uint8_t*)VID_MEMORY;

	size_t i;
	for (i=0; i<(COLS*LINES*2); i+=2) {

		p[i] = ' ';  
		p[i+1] = c;
	}

	_xPos=0;
	_yPos=0;
	_color=c;
	MoveCursor();
}

void SetColor (const size_t c) 
{
	_color=c;
}

size_t GetColor () 
{
	return _color;
}

void GotoXY (uint32_t x, uint32_t y) 
{
	_xPos = x;
	_yPos = y;
	MoveCursor();
}


void MoveCursor()
{
    uint32_t cPos;

    cPos = _yPos * COLS + _xPos;

    OutPort(0x3D4, 14);
    OutPort(0x3D5, cPos >> 8);
    OutPort(0x3D4, 15);
    OutPort(0x3D5, cPos);
}

void OutPort(uint16_t port, uint32_t data)
{
    __asm__ __volatile__ ("out %1, %0" : : "dN" (port), "a" (data));
}

