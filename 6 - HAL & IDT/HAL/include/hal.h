#ifndef _HAL_H_
#define _HAL_H_
//****************************************************************************
//**
//**    hal.h
//**		Hardware Abstraction Layer Interface
//**
//**	The Hardware Abstraction Layer (HAL) provides an abstract interface
//**	to control the basic motherboard hardware devices. This is accomplished
//**	by abstracting hardware dependencies behind this interface.
//**
//**	All routines and types are declared extern and must be defined within
//**	external libraries to define specific hal implimentations.
//**
//****************************************************************************

//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================

#include <stdint.h>

//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================

#define interrupt
#define far
#define near

//============================================================================
//    INTERFACE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================

/* initialize hardware abstraction layer */
extern	uint32_t hal_initialize ();

/* shutdown hardware abstraction layer */
extern	uint32_t hal_shutdown ();

//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================
//****************************************************************************
//**
//**    END [hal.h]
//**
//****************************************************************************
#endif
