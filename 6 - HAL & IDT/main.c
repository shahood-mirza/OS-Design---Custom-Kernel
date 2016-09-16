#include <hal.h>

extern void DivZero();

int KernelMain () 
{

	//Initial message
	
	SetColor(0x16);
	DisplayString("\nInitializing HAL... ");
	uint32_t rval = hal_initialize();
	if (rval != 0)
	{
		SetColor(0x14);
		DisplayString("Failed!");
		for (;;);
	}
	SetColor(0x12);
	DisplayString("Done!\n\n");

	//Three possible commands to test interrupts
	//asm volatile ("int $0x10");
	//__asm__ __volatile__ ("sti");
	DivZero();


	//The kernel's endless loop
	for (;;);

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
