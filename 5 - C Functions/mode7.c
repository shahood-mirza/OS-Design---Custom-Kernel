#include <stdarg.h>
#include <mode7.h>

//some custom variables that were used
static uint32_t _xPos = 0;
static uint32_t _yPos = 0;
static size_t _color = 0;

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
	uint8_t* to_memory = (uint8_t*) VID_MEMORY;
	uint8_t* from_memory = to_memory + 2*COLS;
	
	//take i+1 and copy to row i (for each row)

	do
	{
		*to_memory++ = *from_memory++;
	} while(from_memory >= (uint8_t*) VID_MEMORY && to_memory >= (uint8_t*) VID_MEMORY && from_memory < (uint8_t*) VID_MEMORY + 2*COLS*LINES && to_memory < from_memory);

	//clear final row
	
	do	
	{
		*to_memory++ = (uint8_t) ' ';
		*to_memory++ = (uint8_t) 0x1F;
	} while(to_memory >= (uint8_t*) VID_MEMORY && to_memory < (uint8_t*) VID_MEMORY + 2*COLS*LINES);
	
	if (_yPos > 0)
	{
		_yPos--;
	}
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
	uint8_t* video_memory = (uint8_t*) VID_MEMORY;
	
	// clear screen by filling video memory with spaces
	do 
	{
		*video_memory++ = (uint8_t) ' ';
		*video_memory++ = c;
	} while (video_memory >= (uint8_t*) VID_MEMORY && video_memory < (uint8_t*) VID_MEMORY + 2*LINES*COLS);
}

void SetColor (const size_t c) 
{
	_color = c;
}

size_t GetColor () 
{
	return _color;
}

void GotoXY (uint32_t x, uint32_t y) 
{
	//move write position to x,y
	_xPos = x;
	_yPos = y;
	MoveCursor();
}


void MoveCursor()
{
    uint32_t cPos;

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


//the following get functions are used to retreive values
//since we are only getting the values, we only need to use "return var"

uint32_t GetWidth()
{
	return COLS;
}

uint32_t GetHeight()
{
	return LINES;
}

uint32_t GetXPos()
{
	return _xPos;
}

uint32_t GetYPos()
{
	return _yPos;
}
