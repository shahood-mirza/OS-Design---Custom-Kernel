
%ifndef __GlobalCFunctions_INC_INCLUDED__
%define __GlobalCFunctions_INC_INCLUDED__



bits 32

;==========================================================
;
; This is a division by zero
;
;==========================================================
global _DivZero

_DivZero:
	xor cx, cx
	mov ax, 0x1
	div cx
	ret


;==========================================================
;
; This will set up the GDT
; This is declared in C as 'extern void gdt_install();'
;
;==========================================================

global _gdt_install   ; Allows the C code to link to this
extern __gdtr         ; Says that '_gdtr' is in another file

_gdt_install:
    lgdt [__gdtr]     ; Load the GDT with our '_gdtr' which is a special pointer
    mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush2   ; 0x08 is the offset to our code segment: Far jump!
flush2:
    ret               ; Returns back to the C code!
		

;==========================================================
;
; This will set up the IDT
; This is declared in C as 'extern void idt_install();'
;
;==========================================================
global _idt_install   ; Allows the C code to link to this
extern __idtr        ; Says that '_idtr' is in another file


_idt_install:
   lidt [__idtr]        ; Load the IDT pointer.
   ret

;==========================================================
;
; Interrupt Handler Routines
; 	32 global functions
;	1 extern C function defined
;	1 common handler stub function
;
;==========================================================

global _isr0
global _isr1
global _isr2
global _isr3
global _isr4
global _isr5
global _isr6
global _isr7
global _isr8
global _isr9
global _isr10
global _isr11
global _isr12
global _isr13
global _isr14
global _isr15
global _isr16
global _isr17
global _isr18
global _isr19
global _isr20
global _isr21
global _isr22
global _isr23
global _isr24
global _isr25
global _isr26
global _isr27
global _isr28
global _isr29
global _isr30
global _isr31

;  0: Divide by 0
_isr0:
    cli
    push byte 0
    push byte 0
    jmp isr_common_stub

;  1: Debugger single step
_isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

;  2: Non-maskable interrupt
_isr2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

;  3: Debugger breakpoint
_isr3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

;  4: Overflow
_isr4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

;  5: Out of bounds 
_isr5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

;  6: Undefined Operation Code (OPCode) instruction
_isr6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

;  7: No coprocessor
_isr7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub

;  8: Double fault
_isr8:
    cli
    push byte 8
    jmp isr_common_stub

;  9: Coprocessor segment overrun
_isr9:
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub

; 10: Invalid Task State Segment (TSS)
_isr10:
    cli
    push byte 10
    jmp isr_common_stub

; 11: Segment not present
_isr11:
    cli
    push byte 11
    jmp isr_common_stub

; 12: Stack fault 
_isr12:
    cli
    push byte 12
    jmp isr_common_stub

; 13: General protection fault
_isr13:
    cli
    push byte 13
    jmp isr_common_stub

; 14: Page fault
_isr14:
    cli
    push byte 14
    jmp isr_common_stub

; 15: Unassigned
_isr15:
    cli
    push byte 0
    push byte 15
    jmp isr_common_stub

; 16: Coprocessor error
_isr16:
    cli
    push byte 0
    push byte 16
    jmp isr_common_stub

; 17: Alignment check 
_isr17:
    cli
    push byte 0
    push byte 17
    jmp isr_common_stub

; 18: Machine check 
_isr18:
    cli
    push byte 0
    push byte 18
    jmp isr_common_stub

; 19: Reserved
_isr19:
    cli
    push byte 0
    push byte 19
    jmp isr_common_stub

; 20: Reserved
_isr20:
    cli
    push byte 0
    push byte 20
    jmp isr_common_stub

; 21: Reserved
_isr21:
    cli
    push byte 0
    push byte 21
    jmp isr_common_stub

; 22: Reserved
_isr22:
    cli
    push byte 0
    push byte 22
    jmp isr_common_stub

; 23: Reserved
_isr23:
    cli
    push byte 0
    push byte 23
    jmp isr_common_stub

; 24: Reserved
_isr24:
    cli
    push byte 0
    push byte 24
    jmp isr_common_stub

; 25: Reserved
_isr25:
    cli
    push byte 0
    push byte 25
    jmp isr_common_stub

; 26: Reserved
_isr26:
    cli
    push byte 0
    push byte 26
    jmp isr_common_stub

; 27: Reserved
_isr27:
    cli
    push byte 0
    push byte 27
    jmp isr_common_stub

; 28: Reserved
_isr28:
    cli
    push byte 0
    push byte 28
    jmp isr_common_stub

; 29: Reserved
_isr29:
    cli
    push byte 0
    push byte 29
    jmp isr_common_stub

; 30: Reserved
_isr30:
    cli
    push byte 0
    push byte 30
    jmp isr_common_stub

; 31: Reserved
_isr31:
    cli
    push byte 0
    push byte 31
    jmp isr_common_stub


; void fault_handler(struct regs *r)
extern _fault_handler

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, _fault_handler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

;==========================================================
;
; Interrupt Request Routines
; 	16 global functions
;	1 extern C function defined
;	1 common handler stub function
;
;==========================================================


global _irq0
global _irq1
global _irq2
global _irq3
global _irq4
global _irq5
global _irq6
global _irq7
global _irq8
global _irq9
global _irq10
global _irq11
global _irq12
global _irq13
global _irq14
global _irq15

; 32: IRQ0
_irq0:
    cli
    push byte 0
    push byte 32
    jmp irq_common_stub

; 33: IRQ1
_irq1:
    cli
    push byte 0
    push byte 33
    jmp irq_common_stub

; 34: IRQ2
_irq2:
    cli
    push byte 0
    push byte 34
    jmp irq_common_stub

; 35: IRQ3
_irq3:
    cli
    push byte 0
    push byte 35
    jmp irq_common_stub

; 36: IRQ4
_irq4:
    cli
    push byte 0
    push byte 36
    jmp irq_common_stub

; 37: IRQ5
_irq5:
    cli
    push byte 0
    push byte 37
    jmp irq_common_stub

; 38: IRQ6
_irq6:
    cli
    push byte 0
    push byte 38
    jmp irq_common_stub

; 39: IRQ7
_irq7:
    cli
    push byte 0
    push byte 39
    jmp irq_common_stub

; 40: IRQ8
_irq8:
    cli
    push byte 0
    push byte 40
    jmp irq_common_stub

; 41: IRQ9
_irq9:
    cli
    push byte 0
    push byte 41
    jmp irq_common_stub

; 42: IRQ10
_irq10:
    cli
    push byte 0
    push byte 42
    jmp irq_common_stub

; 43: IRQ11
_irq11:
    cli
    push byte 0
    push byte 43
    jmp irq_common_stub

; 44: IRQ12
_irq12:
    cli
    push byte 0
    push byte 44
    jmp irq_common_stub

; 45: IRQ13
_irq13:
    cli
    push byte 0
    push byte 45
    jmp irq_common_stub

; 46: IRQ14
_irq14:
    cli
    push byte 0
    push byte 46
    jmp irq_common_stub

; 47: IRQ15
_irq15:
    cli
    push byte 0
    push byte 47
    jmp irq_common_stub

extern _irq_handler

irq_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp

    push eax
    mov eax, _irq_handler
    call eax
    pop eax

    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret


%endif ;__GlobalCFunctions_INC_INCLUDED__
