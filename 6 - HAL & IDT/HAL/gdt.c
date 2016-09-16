
//****************************************************************************
//**
//**    gdt.cpp
//**
//**	global descriptor table (gdt) for i86 processors. This handles
//**	the basic memory map for the system and permission levels
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================

#include <gdt.h>
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


/* Processor gdtr register strucutre */
struct gdtr {

	//! size of gdt
	uint16_t		m_limit;

	//! base address of gdt
	uint32_t		m_base;
} __attribute__((packed));



//============================================================================
//    IMPLEMENTATION REQUIRED EXTERNAL REFERENCES (AVOID)
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE DATA
//============================================================================

/* global descriptor table is an array of descriptors */
static struct gdt_descriptor _gdt [MAX_DESCRIPTORS];

/* gdtr */
struct gdtr _gdtr;

//============================================================================
//    INTERFACE DATA
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTION PROTOTYPES
//============================================================================

/* Install gdtr */
extern void gdt_install ();

//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTIONS
//============================================================================


//============================================================================
//    INTERFACE FUNCTIONS
//============================================================================


/* Setup a descriptor in the Global Descriptor Table */
void gdt_set_descriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran)
{
	if (i > MAX_DESCRIPTORS)
		return;

	//! null out the descriptor
	memset ((void*)&_gdt[i], 0, sizeof (struct gdt_descriptor));

	//! set limit and base addresses
	_gdt[i].baseLo	= (base & 0xffff);
	_gdt[i].baseMid	= ((base >> 16) & 0xff);
	_gdt[i].baseHi	= ((base >> 24) & 0xff);
	_gdt[i].limit	= (limit & 0xffff);

	//! set flags and grandularity bytes
	_gdt[i].access = access;
	_gdt[i].gran = ((limit >> 16) & 0x0f);
	_gdt[i].gran |= gran & 0xf0;
}


/* Initialize GDT */
uint32_t i86_gdt_initialize () {

	//! set up gdtr
	_gdtr.m_limit = (sizeof (struct gdt_descriptor) * MAX_DESCRIPTORS)-1;
	_gdtr.m_base = (uint32_t)&_gdt[0];

	//! set null descriptor
	gdt_set_descriptor(0, 0, 0, 0, 0);

	//! set kernel code descriptor
	gdt_set_descriptor (1,0,0xffffffff,
		i86_GDT_DESC_NOVM+i86_GDT_DESC_READWRITE+i86_GDT_DESC_NODIRECTON+i86_GDT_DESC_CODESEG+i86_GDT_DESC_CODEDATA+i86_GDT_DESC_RING0+i86_GDT_DESC_INMEMORY,
		i86_GDT_DESC_LIMIT+i86_GDT_DESC_32BIT+i86_GDT_DESC_4K);

	//! set kernel data descriptor
	gdt_set_descriptor (2,0,0xffffffff,
		i86_GDT_DESC_NOVM+i86_GDT_DESC_READWRITE+i86_GDT_DESC_NODIRECTON+i86_GDT_DESC_DATASEG+i86_GDT_DESC_CODEDATA+i86_GDT_DESC_RING0+i86_GDT_DESC_INMEMORY,
		i86_GDT_DESC_LIMIT+i86_GDT_DESC_32BIT+i86_GDT_DESC_4K);

	//! set user code descriptor
	gdt_set_descriptor (3,0,0xffffffff,
		i86_GDT_DESC_NOVM+i86_GDT_DESC_READWRITE+i86_GDT_DESC_NODIRECTON+i86_GDT_DESC_CODESEG+i86_GDT_DESC_CODEDATA+i86_GDT_DESC_RING3+i86_GDT_DESC_INMEMORY,
		i86_GDT_DESC_LIMIT+i86_GDT_DESC_32BIT+i86_GDT_DESC_4K);

	//! set user data descriptor
	gdt_set_descriptor (4,0,0xffffffff,
		i86_GDT_DESC_NOVM+i86_GDT_DESC_READWRITE+i86_GDT_DESC_NODIRECTON+i86_GDT_DESC_DATASEG+i86_GDT_DESC_CODEDATA+i86_GDT_DESC_RING3+i86_GDT_DESC_INMEMORY,
		i86_GDT_DESC_LIMIT+i86_GDT_DESC_32BIT+i86_GDT_DESC_4K);


	//! install gdtr
	gdt_install ();

	return 0;
}

//============================================================================
//    INTERFACE CLASS BODIES
//============================================================================
//****************************************************************************
//**
//**    END[gdt.cpp]
//**
//****************************************************************************
