#ifndef _IRQ_H
#define _IRQ_H
//****************************************************************************
//**
//**    IRQ.h
//**
//****************************************************************************

#include <stdint.h>
#include <registers.h>

#define INIT_IRQ_HANDLER(__IRQ_)			\
  GotoXY(0,1);						\
  SetColor(0x4e);					\
  DisplayString((uint8_t*) "IRQ: " #__IRQ_ "\n");	\

//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================
//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================


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

//! Remaps and installs the IRQ
extern uint32_t  i86_irq_initialize(uint16_t codeSel);

//! Installs the specified IRQ
extern void irq_install_handler(uint32_t irq, void (*handler)(struct isrregs *r));

//! Uninstalls the specified IRQ
extern void irq_uninstall_handler(uint32_t irq);

void irq_handler(struct isrregs *r);

//IRQ handlers
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================
//****************************************************************************
//**
//**    END [iRQ.h]
//**
//****************************************************************************
#endif
