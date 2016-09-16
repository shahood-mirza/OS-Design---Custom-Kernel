#ifndef _IDT_H_
#define _IDT_H_
//****************************************************************************
//**
//**    idt.h
//**		Interrupt Descriptor Table. The IDT is responsible for providing
//**	the interface for managing interrupts, installing, setting, requesting,
//**	generating, and interrupt callback managing.
//**
//****************************************************************************


#include <stdint.h>

//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================
//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================

/* i86 defines 256 possible interrupt handlers (0-255) */
#define I86_MAX_INTERRUPTS		256

/* Bits 40-47 of the descriptor */
/* Descriptor type */
#define I86_IDT_DESC_INT16		0x06	//___00110
#define I86_IDT_DESC_INT32		0x0E	//___01110
#define I86_IDT_DESC_TRAP16		0x07	//___00111
#define I86_IDT_DESC_TRAP32		0x0F	//___01111
#define I86_IDT_DESC_TASK		0x05	//___00101
/* Descriptor privilege ring */
#define I86_IDT_DESC_RING0		0x00	//_00_____
#define I86_IDT_DESC_RING1		0x20	//_01_____
#define I86_IDT_DESC_RING2		0x40	//_10_____
#define I86_IDT_DESC_RING3		0x60	//_11_____
/* Descriptor present/abscent */
#define I86_IDT_DESC_PRESENT		0x80	//1_______
#define I86_IDT_DESC_ABSCENT		0x00	//0_______

/* Interrupt handler rountine data type */
typedef void (*I86_IRQ_HANDLER)(void);

//============================================================================
//    INTERFACE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================


/* interrupt descriptor */
struct idt_descriptor {

	//! bits 0-16 of interrupt routine (ir) address
	uint16_t		baseLo;

	//! code selector in gdt
	uint16_t		sel;

	//! reserved, shold be 0
	uint8_t			reserved;

	//! bit flags. Set with flags above
	uint8_t			flags;

	//! bits 16-32 of ir address
	uint16_t		baseHi;
} __attribute__((packed));


//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================

/* Modify an interrupt descriptor in the IDT */
extern uint32_t i86_install_ir (uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER);

/* Initialize the IDT with the default 256 descriptors */
extern uint32_t i86_idt_initialize (uint16_t codeSel);

//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================
//****************************************************************************
//**
//**    END [idt.h]
//**
//****************************************************************************
#endif
