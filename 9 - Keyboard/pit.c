//****************************************************************************
//**
//**    pit.c
//**		
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================

#include <pit.h>
#include <mode7.h>
#include <stdint.h>
#include <irq.h>
#include <registers.h>

//============================================================================
//    IMPLEMENTATION PRIVATE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================

//============================================================================
//    IMPLEMENTATION PRIVATE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE STRUCTURES / UTILITY CLASSES
//============================================================================

//============================================================================
//    IMPLEMENTATION REQUIRED EXTERNAL REFERENCES (AVOID)
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE DATA
//============================================================================


//The time
uint32_t hours = 0;
uint32_t minutes = 0;
uint32_t seconds = 0;
uint32_t ticks = 0;


//============================================================================
//    INTERFACE DATA
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTION PROTOTYPES
//============================================================================

void pit_handler(struct isrregs *r);
void display_time();

//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTIONS
//============================================================================
void pit_handler(struct isrregs *r)
{
   ticks++;

   if (ticks == 18)
   {
		ticks = 0;
		seconds++;
		if (seconds == 60)
		{
			seconds = 0;
			minutes++;
			if (minutes == 60)
			{
				minutes = 0;
				hours++;
			}
		}
		display_time();
   }
}

//Display time in HHH:MM.SS
//top-right corner, on a coloured line
void display_time()
{
	//save current colour & position
	size_t colour = GetColor();
	uint32_t xpos = GetXPos();
	uint32_t ypos = GetYPos();

	//set to black on white, not blinking
	SetColor(0x0E);
	//goto top-left
	GotoXY (0,0);

	uint8_t timeStart = GetWidth() - 9;
	uint8_t i;
	for (i = 0; i < timeStart; i++)
		DisplayChar(' ');

	//display hours
	if (hours < 100)
		DisplayInteger(0);
	if (hours < 10)
		DisplayInteger(0);
	DisplayInteger(hours);
	DisplayChar(':');
	//diplay minutes
	if (minutes < 10)
		DisplayInteger(0);
	DisplayInteger(minutes);
	DisplayChar('.');
	//display seconds
	if (seconds < 10)
		DisplayInteger(0);
	DisplayInteger(seconds);

	//restore previous colour & position
	SetColor (colour);
	GotoXY (xpos, ypos);
}



//============================================================================
//    INTERFACE FUNCTIONS
//============================================================================


void install_timer()
{
	irq_install_handler(0, &pit_handler);
	hours = 0;
	minutes = 0;
	seconds = 0;
	ticks = 0;

}


//============================================================================
//    INTERFACE CLASS BODIES
//============================================================================
//****************************************************************************
//**
//**    END[pic.c]
//**
//****************************************************************************
