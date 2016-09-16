
org 0x100000   ; Kernel starts at 1 MB

bits 32    ; 32 bit code

jmp KernelCode   ; jump to entry point

%include "KernelDisplay.inc"

msg db  0x0A, 0x0A, "ENGI3655 32-Bit Kernel", 0x0A, 0

KernelCode:

 ;-------------------------------;
 ;   Set registers               ;
 ;-------------------------------;

 mov ax, 0x10  ; set data segments to data selector (0x10)
 mov ds, ax
 mov ss, ax
 mov es, ax
 mov esp, 90000h  ; stack begins from 90000h

 ;---------------------------------------;
 ;   Clear screen and print success      ;
 ;---------------------------------------;

 call ClearScreen
 mov ebx, msg
 call DisplayString

 ;---------------------------------------;
 ;   Kernel's Infinite loop              ;
 ;---------------------------------------;
	
 jmp $

 ;---------------------------------------;
 ;   Stop execution                      ;
 ;   (OS shutdown)                       ;
 ;---------------------------------------;

 cli
 hlt



