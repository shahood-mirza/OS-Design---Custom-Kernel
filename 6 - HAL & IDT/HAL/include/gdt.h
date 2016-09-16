#ifndef _GDT_H_
#define _GDT_H_
//****************************************************************************
//**
//**    gdt.h
//**
//**	global descriptor table (gdt) for 80x86 processors. This handles
//**	the basic memory map for the system and permission levels
//**
//**	The system software should have a gdt set up prior to this being
//**	used. This sets up a basic gdt interface that can be managed through
//**	the HAL
//**
//****************************************************************************

//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================

#include <stdint.h>

//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================

/* maximum amount of descriptors allowed */
#define MAX_DESCRIPTORS				5

/***	 gdt descriptor access bit flags.	***/

/*Bit 0: Virtual memory access bit */
#define i86_GDT_DESC_NOVM			0x0000			//_______0
#define i86_GDT_DESC_VM				0x0001			//_______1

/*Bit 1: Segment read-only or read-write */
#define i86_GDT_DESC_READONLY			0x0000			//______0_
#define i86_GDT_DESC_READWRITE			0x0002			//______1_

/*Bit 2: Expansion direction (Data segments), conforming (Code Segments)*/
#define i86_GDT_DESC_NODIRECTON			0x0000			//_____0__
#define i86_GDT_DESC_EXPANSION			0x0004			//_____1__
#define i86_GDT_DESC_CONFORMING			0x0004			//_____1__

/*Bit 3: Executable segment*/
#define i86_GDT_DESC_DATASEG			0x0000			//____0___
#define i86_GDT_DESC_CODESEG			0x0008			//____1___

/*Bit 4: Descriptor Bit */
#define i86_GDT_DESC_SYSTEM			0x0000			//___0____
#define i86_GDT_DESC_CODEDATA			0x0010			//___1____

/*Bits 5-6: Privilege*/
#define i86_GDT_DESC_RING0			0x0000			//_00_____
#define i86_GDT_DESC_RING1			0x0020			//_01_____
#define i86_GDT_DESC_RING2			0x0040			//_10_____
#define i86_GDT_DESC_RING3			0x0060			//_11_____

/*Bit 7: Segment in memory */
#define i86_GDT_DESC_NOTINMEMORY		0x0000			//0_______
#define i86_GDT_DESC_INMEMORY			0x0080			//1_______



/***	gdt descriptor granularity bit flags	***/

/*Bits 0-3: Segment limit */
#define i86_GDT_DESC_LIMIT			0x0f			//____1111

/*Bits 4-5: Reserved */							//__00____

/*Bits 6: Segment type  */
#define i86_GDT_DESC_16BIT			0x00			//_0______
#define i86_GDT_DESC_32BIT			0x40			//_1______

/*Bits 7: Granularity */
#define i86_GDT_DESC_1B				0x00			//0_______
#define i86_GDT_DESC_4K				0x80			//1_______

//============================================================================
//    INTERFACE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================


/* GDT descriptor */
struct gdt_descriptor {

	//! bits 0-15 of segment limit
	uint16_t		limit;

	//! bits 0-23 of base address
	uint16_t		baseLo;
	uint8_t			baseMid;

	//! descriptor access flags
	uint8_t			access;

	uint8_t			gran;

	//! bits 24-32 of base address
	uint8_t			baseHi;
} __attribute__((packed));


//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================

/* Setup a descriptor in the GDT */
extern void gdt_set_descriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran);

/* Initializes GDT */
extern	uint32_t i86_gdt_initialize ();

//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================
//****************************************************************************
//**
//**    END [gdt.h]
//**
//****************************************************************************
#endif
