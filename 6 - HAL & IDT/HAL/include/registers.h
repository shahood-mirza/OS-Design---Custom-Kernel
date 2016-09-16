#ifndef _REGISTERS_H_
#define _REGISTERS_H_
//****************************************************************************
//**
//**    registers.h
//**
//**	processor register structures and declarations. This interface abstracts
//**	register names behind a common, portable interface
//**
//****************************************************************************


//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================

#include <stdint.h>

//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
//============================================================================
//    INTERFACE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================

//! 32 bit registers
struct _R32BIT {
    uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp, eflags;
    uint8_t cflag;
};

//! 16 bit registers
struct _R16BIT {
    uint16_t ax, bx, cx, dx, si, di, bp, sp, es, cs, ss, ds, flags;
    uint8_t cflag;
};

//! 16 bit registers expressed in 32 bit registers
struct _R16BIT32 {
    uint16_t ax, axh, bx, bxh, cx, cxh, dx, dxh;
	uint16_t si, di, bp, sp, es, cs, ss, ds, flags;
	uint8_t cflags;
};

//! 8 bit registers
struct _R8BIT {
    uint8_t al, ah, bl, bh, cl, ch, dl, dh;
};

//! 8 bit registers expressed in 32 bit registers
struct _R8BIT32 {
    	uint8_t al, ah; uint16_t axh; 
	uint8_t bl, bh; uint16_t bxh; 
	uint8_t cl, ch; uint16_t cxh; 
	uint8_t dl, dh; uint16_t dxh; 
};

//! 8 and 16 bit registers union
union _INTR16 {
    struct _R16BIT x;
    struct _R8BIT h;
};

//! 32 bit, 16 bit and 8 bit registers union
union _INTR32 {
	struct _R32BIT x;
	struct _R16BIT32 l;
	struct _R8BIT32 h;
};

//! The registers (in stack order) during an ISR
struct isrregs
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};


//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================
//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================
//****************************************************************************
//**
//**    END [registers.h]
//**
//****************************************************************************
#endif
