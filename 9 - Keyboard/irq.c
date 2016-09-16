//****************************************************************************
//**
//**    Irq.c
//**		
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================

#include <irq.h>
#include <idt.h>
#include <mode7.h>
#include <port.h>

//============================================================================
//    IMPLEMENTATION PRIVATE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================

#define I86_MASTER_PIC_COMMAND_PORT	0x20
#define I86_MASTER_PIC_DATA_PORT	0x21
#define I86_SLAVE_PIC_COMMAND_PORT	0xA0
#define I86_SLAVE_PIC_DATA_PORT		0xA1

#define I86_PIC_ICW_BEGIN_INITIALIZATION	0x11
#define I86_PIC_ICW_SET_8086_MODE		0x01
#define I86_PIC_ICW_MASTER_IR_FROM_SLAVE	0x04
#define I86_PIC_ICW_SLAVE_IR_TO_MASTER		0x02
#define I86_PIC_END_OF_INTERRUPT		0x20

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


//! An array of function pointers. We use this to handle custom IRQ handlers for a given IRQ 
void *irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

uint8_t *IRQNum[] =
{
    "0 (Programmable Interval Timer)",
    "1 (Keyboard controller)",
    "2 (Cascade to slave PIC)",
    "3 (Serial Port COM2)",
    "4 (Serial Port COM1)",
    "5 (Parallel Port LPT2)",
    "6 (Floppy disk controller)",
    "7 (Parallel Port LPT1)",
    "8 (Real-Time Clock)",
    "9 (CGA Retrace)",
    "10 (Reserved)",
    "11 (Reserved)",
    "12 (PS/2 Mouse controller)",
    "13 (Math coprocessor)",
    "14 (Hard disk controller 1)",
    "15 (Hard disk controller 2)"
};

//============================================================================
//    INTERFACE DATA
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTION PROTOTYPES
//============================================================================


//! 16 IRQs in Assembly
extern void irq0 ();
extern void irq1 ();
extern void irq2 ();
extern void irq3 ();
extern void irq4 ();
extern void irq5 ();
extern void irq6 ();
extern void irq7 ();
extern void irq8 ();
extern void irq9 ();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

//! Handler for the first 32 isr, to display a message
void irq_handler(struct isrregs *r);


//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTIONS
//============================================================================

void irq_handler(struct isrregs *r)
{


    /* This is a blank function pointer */
    void (*handler)(struct isrregs *r);

    /* Find out if we have a custom handler to run for this
    *  IRQ, and then finally, run it */
    handler = irq_routines[r->int_no - 32];
    if ( (uint32_t)handler != 0 )
    {
        handler(r);
    }
    else
    {
        ClearScreen(0x18);
        GotoXY(0,0);
        SetColor(0x1e);
        DisplayString("Unhandled IRQ #");
        DisplayString(IRQNum[r->int_no - 32]);
        DisplayString(" detected.");
    }

    /* If the IDT entry that was invoked was greater than 40
    *  (meaning IRQ8 - 15), then we need to send an EOI to
    *  the slave controller */
    if (r->int_no >= 40)
    {
        outport(I86_SLAVE_PIC_COMMAND_PORT, I86_PIC_END_OF_INTERRUPT);
    }

    /* In either case, we need to send an EOI to the master
    *  interrupt controller too */
    outport(I86_MASTER_PIC_COMMAND_PORT, I86_PIC_END_OF_INTERRUPT);

}


//============================================================================
//    INTERFACE FUNCTIONS
//============================================================================


//! remaps the IRQ and installs the handlers
uint32_t i86_irq_initialize(uint16_t codeSel)
{

    //Initialization Control Word 1 (ICW1): begin!
    outport( I86_MASTER_PIC_COMMAND_PORT, I86_PIC_ICW_BEGIN_INITIALIZATION);
    outport( I86_SLAVE_PIC_COMMAND_PORT,  I86_PIC_ICW_BEGIN_INITIALIZATION);

    //Initialization Control Word 2 (ICW2): set new offsets
    outport( I86_MASTER_PIC_DATA_PORT,    0x20); 			    //0x20 = 32, INT of IRQ0
    outport( I86_SLAVE_PIC_DATA_PORT,     0x28); 			    //0x28 = 40, INT of IRQ8

    //Initialization Control Word 3 (ICW3): set master-slave connection
    outport( I86_MASTER_PIC_DATA_PORT,    I86_PIC_ICW_MASTER_IR_FROM_SLAVE);
    outport( I86_SLAVE_PIC_DATA_PORT,     I86_PIC_ICW_SLAVE_IR_TO_MASTER  );

    //Initialization Control Word 4 (ICW4): enables 8086 mode
    outport( I86_MASTER_PIC_DATA_PORT,    I86_PIC_ICW_SET_8086_MODE);
    outport( I86_SLAVE_PIC_DATA_PORT,     I86_PIC_ICW_SET_8086_MODE);

    //Clear port
    outport( I86_MASTER_PIC_DATA_PORT,    0x0);
    outport( I86_SLAVE_PIC_DATA_PORT,     0x0);

    i86_install_ir(32, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq0);
    i86_install_ir(33, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq1);
    i86_install_ir(34, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq2);
    i86_install_ir(35, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq3);
    i86_install_ir(36, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq4);
    i86_install_ir(37, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq5);
    i86_install_ir(38, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq6);
    i86_install_ir(39, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq7);
    i86_install_ir(40, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq8);
    i86_install_ir(41, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq9);
    i86_install_ir(42, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq10);
    i86_install_ir(43, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq11);
    i86_install_ir(44, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq12);
    i86_install_ir(45, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq13);
    i86_install_ir(46, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq14);
    i86_install_ir(47, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)irq15);


    uint32_t i = 0;
    for (i = 0; i < 16; i++)
        irq_uninstall_handler(i);

    return 0;
}

/* This installs a custom IRQ handler for the given IRQ */
void irq_install_handler(uint32_t irq, void (*handler)(struct isrregs *r))
{
    irq_routines[irq] = handler;
}

/* This clears the handler for a given IRQ */
void irq_uninstall_handler(uint32_t irq)
{
    irq_routines[irq] = 0;
}

//============================================================================
//    INTERFACE CLASS BODIES
//============================================================================
//****************************************************************************
//**
//**    END[irq.c]
//**
//****************************************************************************
