#include <mode7.h>
#include <hal.h>

extern void DivZero();

int KernelMain () 
{

	ClearScreen(0x18);
	GotoXY(4,10);
	SetColor(0x1D);
	DisplayString("+-----------------------------------------+\n");
	DisplayString("    |   ");
	SetColor(0x1F);
	DisplayString("ENGI3655 32-Bit C Kernel Executing!");
	SetColor(0x1D);
	DisplayString("   |\n");
	DisplayString("    +-----------------------------------------+\n\n");
	
	SetColor(0x16);
	DisplayString("Initializing HAL... ");
	uint32_t rval = hal_initialize();
	if (rval != 0)
	{
		SetColor(0x14);
		DisplayString("Failed!");
		for (;;);
	}
	SetColor(0x12);
	DisplayString("Done!\n\n");

	asm volatile ("int $0x10");
	//__asm__ __volatile__ ("sti");
	//DivZero();



	///////////////////////////////////////	
	//ENDLESS LOOP
	for (;;);
	///////////////////////////////////////

	//OS Shutdown code
	DisplayString("Shutting down HAL... ");
	uint32_t rval2 = hal_shutdown();
	if (rval2 != 0)
	{
		SetColor(0x14);
		DisplayString("Failed!");
		for (;;);
	}
	SetColor(0x12);
	DisplayString("Done!\n\n");

    	return 0;
}
