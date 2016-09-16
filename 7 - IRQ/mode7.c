#include <stdarg.h>
#include <mode7.h>
#include <port.h>

#define VID_MEMORY	0xB8000
#define	COLS		80
#define	LINES		25


static uint8_t _xPos=0, _yPos=0;
static uint8_t _color=0;

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
		if (_yPos >= LINES)
			ScrollUp();
		MoveCursor();
		return;
	}

	if (_xPos >= COLS)
	{
		_yPos++;
		_xPos=0;
		if (_yPos >= LINES)
			ScrollUp();
	}

	uint8_t* p = (uint8_t*)VID_MEMORY + (_xPos++)*2 + _yPos * COLS * 2;
	*p++ = c;
	*p =_color;
	MoveCursor();

}

void ScrollUp()
{
	uint8_t* p = (uint8_t*)VID_MEMORY;

	//Copy each character back one line (COLS characters)
	size_t i;
	for (i=0; i<(COLS*(LINES-1)*2); i++) 
		p[i] = p[i+(COLS*2)];  

	//Clear the last line
	for (i=COLS*(LINES-1)*2; i<(COLS*LINES*2); i+=2) {

		p[i] = ' ';  
		p[i+1] = _color;
	}

	//update position
	_yPos = LINES-1;
}

void DisplayString(uint8_t *str)
{
	if (!str)
		return;

	size_t i;
	for (i=0; i < strlen(str); i++)
		DisplayChar(str[i]);
}


void ClearScreen (const uint8_t c) 
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

void SetColor (const uint8_t c) 
{
	_color=c;
}

uint8_t GetColor () 
{
	return _color;
}

void GotoXY (uint8_t x, uint8_t y) 
{
	_xPos = x;
	_yPos = y;
	MoveCursor();
}


void MoveCursor()
{
    uint16_t cPos;

    cPos = _yPos * COLS + _xPos;

    outport(0x3D4, 14);
    outport(0x3D5, cPos >> 8);
    outport(0x3D4, 15);
    outport(0x3D5, cPos);
}



void DisplayInteger(uint32_t n)
{

    if (n == 0)
    {
        DisplayChar('0');
        return;
    }

    uint32_t acc = n;
    uint8_t c[32];
    uint32_t i = 0;
    while (acc > 0)
    {
        c[i] = '0' + acc%10; 
        acc /= 10;
        i++;
    }
    i--;

    while (1)
    {
		DisplayChar(c[i]);
		if (i == 0)
			break;
		i--;
    }

}

uint8_t GetWidth()
{
	return COLS;
}

uint8_t GeHeight()
{
	return LINES;
}

uint8_t GetXPos()
{
	return _xPos;
}

uint8_t GetYPos()
{
	return _yPos;
}
