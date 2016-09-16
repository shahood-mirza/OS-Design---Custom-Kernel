//****************************************************************************
//**
//**    idt.cpp
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

//============================================================================
//    IMPLEMENTATION PRIVATE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE STRUCTURES / UTILITY CLASSES
//============================================================================


/* The processor's idtr register structure */
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

/* interrupt descriptor table */
static struct idt_descriptor	_idt [I86_MAX_INTERRUPTS];

/* idtr */
struct idtr				_idtr;

//============================================================================
//    INTERFACE DATA
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTION PROTOTYPES
//============================================================================

/* installs idtr into processors idtr register */
extern void idt_install ();

/* default int handler used to catch unregistered interrupts */
static void i86_default_handler ();

//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTIONS
//============================================================================



/* default handler to catch unhandled system interrupts. */
static void i86_default_handler () {

	ClearScreen(0x18);
	GotoXY(0,1);
	SetColor(0x1e);
	DisplayString("*** [i86 Hal] i86_default_handler: Unhandled Interrupt ***");

	for(;;);
}

//============================================================================
//    INTERFACE FUNCTIONS
//============================================================================


/* Modify an interrupt descriptor in the IDT */
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


/* Initialize the IDT with the default 256 descriptors */
uint32_t i86_idt_initialize (uint16_t codeSel) {

	//! set up idtr for processor
	_idtr.limit = sizeof (struct idt_descriptor) * I86_MAX_INTERRUPTS -1;
	_idtr.base	= (uint32_t)&_idt[0];

	//! null out the idt
	memset ((void*)&_idt[0], 0, sizeof (struct idt_descriptor) * I86_MAX_INTERRUPTS-1);

	//! register default handlers
	size_t i;
	for (i=0; i<I86_MAX_INTERRUPTS; i++)
		i86_install_ir (i, I86_IDT_DESC_PRESENT+I86_IDT_DESC_RING0+I86_IDT_DESC_INT32,
			codeSel, (I86_IRQ_HANDLER)i86_default_handler);

	//! install our idt
	idt_install ();

	return 0;
}

//============================================================================
//    INTERFACE CLASS BODIES
//============================================================================
//****************************************************************************
//**
//**    END[idt.cpp]
//**
//****************************************************************************
