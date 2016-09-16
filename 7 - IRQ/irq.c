//included files
#include <IRQ.h>
#include <port.h>

#include <idt.h>
#include <port.h>
#include <mode7.h>

//some defined parameters
#define IRQ_PRIMARY_PIC_COMMAND_PORT 0x20
#define IRQ_PRIMARY_PIC_DATA_PORT 0x21

#define IRQ_SECONDARY_PIC_COMMAND_PORT 0xA0
#define IRQ_SECONDARY_PIC_DATA_PORT 0xA1

#define IRQ_ICW1 0x11
#define IRQ_ICW2_PRIMARY_PIC 32
#define IRQ_ICW2_SECONDARY_PIC 40
#define IRQ_ICW3_PRIMARY_PIC 0x04
#define IRQ_ICW3_SECONDARY_PIC 0x02
#define IRQ_ICW4 0x01

#define IRQ_COUNT 16


#define EOF_PIC_DEINIT(IRQ)					\
  outport(IRQ_PRIMARY_PIC_COMMAND_PORT, 0x20);			\
  if ((IRQ)>7) outport(IRQ_SECONDARY_PIC_COMMAND_PORT, 0x20);



void *irq_routines[IRQ_COUNT];


//primary functions
void i86_irq_initialize(uint16_t codeSel) {
  uint8_t i;
  for(i = 0; i < IRQ_COUNT; i++)
    {
      irq_routines[i] = 0;
    }

  //mapping IRQ 0-15 to interrupts 32-47
  outport(IRQ_PRIMARY_PIC_COMMAND_PORT, IRQ_ICW1);
  outport(IRQ_SECONDARY_PIC_COMMAND_PORT, IRQ_ICW1);

  outport(IRQ_PRIMARY_PIC_DATA_PORT, IRQ_ICW2_PRIMARY_PIC);
  outport(IRQ_SECONDARY_PIC_DATA_PORT, IRQ_ICW2_SECONDARY_PIC);

  outport(IRQ_PRIMARY_PIC_DATA_PORT, IRQ_ICW3_PRIMARY_PIC);
  outport(IRQ_SECONDARY_PIC_DATA_PORT, IRQ_ICW3_SECONDARY_PIC);

  outport(IRQ_PRIMARY_PIC_DATA_PORT, IRQ_ICW4);
  outport(IRQ_SECONDARY_PIC_DATA_PORT, IRQ_ICW4);

  //zero data port
  outport(IRQ_PRIMARY_PIC_DATA_PORT, 0);
  outport(IRQ_SECONDARY_PIC_DATA_PORT, 0);

  
  uint16_t _flags = I86_IDT_DESC_PRESENT + I86_IDT_DESC_RING0 + I86_IDT_DESC_INT32;
  //setting up default IDT for IRQs
  i86_install_ir(32, _flags, codeSel, (I86_IRQ_HANDLER) irq0);
  i86_install_ir(33, _flags, codeSel, (I86_IRQ_HANDLER) irq1);
  i86_install_ir(34, _flags, codeSel, (I86_IRQ_HANDLER) irq2);
  i86_install_ir(35, _flags, codeSel, (I86_IRQ_HANDLER) irq3);
  i86_install_ir(36, _flags, codeSel, (I86_IRQ_HANDLER) irq4);
  i86_install_ir(37, _flags, codeSel, (I86_IRQ_HANDLER) irq5);
  i86_install_ir(38, _flags, codeSel, (I86_IRQ_HANDLER) irq6);
  i86_install_ir(39, _flags, codeSel, (I86_IRQ_HANDLER) irq7);
  i86_install_ir(40, _flags, codeSel, (I86_IRQ_HANDLER) irq8);
  i86_install_ir(41, _flags, codeSel, (I86_IRQ_HANDLER) irq9);
  i86_install_ir(42, _flags, codeSel, (I86_IRQ_HANDLER) irq10);
  i86_install_ir(43, _flags, codeSel, (I86_IRQ_HANDLER) irq11);
  i86_install_ir(44, _flags, codeSel, (I86_IRQ_HANDLER) irq12);
  i86_install_ir(45, _flags, codeSel, (I86_IRQ_HANDLER) irq13);
  i86_install_ir(46, _flags, codeSel, (I86_IRQ_HANDLER) irq14);
  i86_install_ir(47, _flags, codeSel, (I86_IRQ_HANDLER) irq15);
}

void irq_install_handler(uint32_t irq, 
			 void (*handler)(struct isrregs *r)) { 
  irq_routines[irq] = handler; 
}

void irq_handler(struct isrregs *r) {
        uint8_t irq_no = r->int_no - 32;
        
        //initialize function pointer
        void (*handler)(struct isrregs *r);

        if (irq_no < IRQ_COUNT)
        {
                handler = irq_routines[irq_no];
                if ((uint32_t) handler != 0)
                {
                        handler(r);
                }
                else
                {
                        //default IRQ handler
                        DisplayString((uint8_t*) "Handler for IRQ ");
                        DisplayInteger(irq_no);
                        DisplayString((uint8_t*) " not set.");
                }


                //send end of interrupt (EOI)
                outport(IRQ_PRIMARY_PIC_COMMAND_PORT, 0x20);

                if(irq_no > 7)
                { outport(IRQ_SECONDARY_PIC_COMMAND_PORT, 0x20);}
                //end of EOI

        }
        else
        {
                DisplayString((uint8_t*) "IRQ ");
                DisplayInteger(irq_no);
                DisplayString((uint8_t*) " is an invalid IRQ number.");
        }
}

void irq_uninstall_handler(uint32_t irq) { 
  irq_routines[irq] = 0; 
}
