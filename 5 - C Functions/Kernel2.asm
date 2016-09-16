
;org 0x100000   ; Kernel starts at 1 MB

bits 32    ; 32 bit code

global start
start:
jmp KernelCode   ; jump to entry point

%include "KernelDisplay.inc"

msg db  0x0A, 0x0A, "ENGI3558 32-Bit Kernel", 0x0A, 0
msg2 db  0x0A, 0x0A, "Now exiting Kernel... Goodbye!", 0x0A, 0

KernelCode:

 ;-------------------------------;
 ;   Set registers  ;
 ;-------------------------------;

 mov ax, 0x10  ; set data segments to data selector (0x10)
 mov ds, ax
 mov ss, ax
 mov es, ax
 mov esp, 90000h  ; stack begins from 90000h

 ;---------------------------------------;
 ;   Clear screen and print success ;
 ;---------------------------------------;

 call ClearScreen
 mov ebx, msg
 call DisplayString


 extern _KernelMain
 call _KernelMain
	

 ;---------------------------------------;
 ;   Stop execution   ;
 ;---------------------------------------;

 mov ebx, msg2
 call DisplayString
 cli
 hlt



