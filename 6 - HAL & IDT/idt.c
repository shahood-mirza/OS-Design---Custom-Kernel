//****************************************************************************
//**
//**    Idt.cpp
//**		Interrupt Descriptor Table. The IDT is responsible for providing
//**	the interface for managing interrupts, installing, setting, requesting,
//**	generating, and interrupt callback managing.
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================

#include <idt.h>
#include <hal.h>
#include <mode7.h>
#include <mem.h>
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


//! describes the structure for the processors idtr register
struct idtr {

	//! size of the interrupt descriptor table (idt)
	uint16_t		limit;

	//! base address of idt
	uint32_t		base;
} __attribute__((packed));

//============================================================================
//    IMPLEMENTATION REQUIRED EXTERNAL REFERENCES (AVOID)
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE DATA
//============================================================================
//! An array of function pointers. We use this to handle custom handlers for a given interrupt
void *idt_routines[32];

//! interrupt descriptor table
static struct idt_descriptor	_idt [I86_MAX_INTERRUPTS];

//! idtr structure used to help define the cpu's idtr register
struct idtr				_idtr;

//! Array of ISR messages
uint8_t *exception_messages[] =
{
    "0: Divide by zero",
    "1: Debugger single step",
    "2: Non-maskable interrupt",
    "3: Debugger breakpoint",
    "4: Overflow",
    "5: Out of bounds",
    "6: Undefined OPCode instruction",
    "7: No coprocessor",
    "8: Double fault",
    "9: Coprocessor segment overrun",
    "10: Invalid TSS",
    "11: Segment not present",
    "12: Stack fault",
    "13: General protection fault",
    "14: Page fault",
    "15: Unknown Interrupt",
    "16: Coprocessor error",
    "17: Alignment check",
    "18: Machine check",
    "19: Reserved",
    "20: Reserved",
    "21: Reserved",
    "22: Reserved",
    "23: Reserved",
    "24: Reserved",
    "25: Reserved",
    "26: Reserved",
    "27: Reserved",
    "28: Reserved",
    "29: Reserved",
    "30: Reserved",
    "31: Reserved"
};


//============================================================================
//    INTERFACE DATA
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTION PROTOTYPES
//============================================================================

//! installs idtr into processors idtr register
extern void idt_install ();

//! default int handler used to catch unregistered interrupts
static void i86_default_handler ();

//! default int handler used to catch unregistered interrupts
void fault_handler(struct isrregs *r);

//! 32 ISRs in Assembly
extern void isr0 ();
extern void isr1 ();
extern void isr2 ();
extern void isr3 ();
extern void isr4 ();
extern void isr5 ();
extern void isr6 ();
extern void isr7 ();
extern void isr8 ();
extern void isr9 ();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTIONS
//============================================================================



//! default handler to catch unhandled system interrupts
static void i86_default_handler () {

	ClearScreen(0x18);
	GotoXY(0,0);
	SetColor(0x1e);
	DisplayString("*** [i86 Hal] i86_default_handler: Unhandled Interrupt ***");

	for(;;);
}

//! Handler for the first 32 interrupts
void fault_handler(struct isrregs *r)
{
    void (*handler)(struct isrregs *r);
    if (r->int_no < 32)
    {
	    handler = idt_routines[r->int_no];
	    if ( (uint32_t)handler != 0 )
	    {
	        handler(r);
	    }
	    else
	    {
		ClearScreen(0x18);
		GotoXY(0,0);
		SetColor(0x1e);
		DisplayString("Interrupt #");
		DisplayString(exception_messages[r->int_no]);
		DisplayString(" error.\nSystem Halted");
	        for (;;);
	    }

    }
    i86_default_handler ();
}

//============================================================================
//    INTERFACE FUNCTIONS
//============================================================================


//! installs a new interrupt handler
uint32_t i86_install_ir (uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER irq) {

	if (i>I86_MAX_INTERRUPTS)
		return 1;

	if (!irq)
		return 1;

	//! get base address of interrupt handler
	uint64_t		uiBase = (uint64_t)&(*irq);

	//! store base address into idt
	_idt[i].baseLo		=	(uiBase & 0xffff);
	_idt[i].baseHi		=	((uiBase >> 16) & 0xffff);
	_idt[i].reserved	=	0;
	_idt[i].flags		=	(flags);
	_idt[i].sel		=	sel;

	return	0;
}


//! initialize idt
uint32_t i86_idt_initialize (uint16_t codeSel) {

	//! set up idtr for processor
	_idtr.limit = sizeof (struct idt_descriptor) * I86_MAX_INTERRUPTS -1;
	_idtr.base	= (uint32_t)&_idt[0];

	//! null out the idt
	memset ((void*)&_idt[0], 0, sizeof (struct idt_descriptor) * I86_MAX_INTERRUPTS-1);

	//! First 32 ISRs
	i86_install_ir(0, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr0);
	i86_install_ir(1, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr1);
	i86_install_ir(2, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr2);
	i86_install_ir(3, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr3);
	i86_install_ir(4, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr4);
	i86_install_ir(5, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr5);
	i86_install_ir(6, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr6);
	i86_install_ir(7, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr7);
	i86_install_ir(8, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr8);
	i86_install_ir(9, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr9);
	i86_install_ir(10, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr10);
	i86_install_ir(11, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr11);
	i86_install_ir(12, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr12);
	i86_install_ir(13, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr13);
	i86_install_ir(14, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr14);
	i86_install_ir(15, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr15);
	i86_install_ir(16, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr16);
	i86_install_ir(17, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr17);
	i86_install_ir(18, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr18);
	i86_install_ir(19, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr19);
	i86_install_ir(20, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr20);
	i86_install_ir(21, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr21);
	i86_install_ir(22, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr22);
	i86_install_ir(23, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr23);
	i86_install_ir(24, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr24);
	i86_install_ir(25, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr25);
	i86_install_ir(26, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr26);
	i86_install_ir(27, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr27);
	i86_install_ir(28, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr28);
	i86_install_ir(29, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr29);
	i86_install_ir(30, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr30);
	i86_install_ir(31, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)isr31);

	//! register default handlers
	size_t i;
	for (i=0; i<32; i++)
		i86_uninstall_handler(i);
	for (i=32; i<I86_MAX_INTERRUPTS; i++)
		i86_install_ir (i, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)fault_handler);

	//! install our idt
	idt_install ();

	return 0;
}

/* This installs a custom IRQ handler for the given INT */
void i86_install_handler(uint32_t idt, void (*handler)(struct isrregs *r))
{
    idt_routines[idt] = handler;
}

/* This clears the handler for a given INT */
void i86_uninstall_handler(uint32_t idt)
{
    idt_routines[idt] = 0;
}


//============================================================================
//    INTERFACE CLASS BODIES
//============================================================================
//****************************************************************************
//**
//**    END[idt.cpp]
//**
//****************************************************************************
