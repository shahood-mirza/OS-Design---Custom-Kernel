[BITS 16]       ;16-bits real mode
[ORG 0x0000]    ;Origin, tell the assembler that where the code will be in memory after it is been loaded

jmp main

;Included functions
%include "PrintScreen.inc"

;Data
HelloString db "Hello World",10,13,0


main:
mov ax, 1000h
MOV DS, AX
MOV ES, AX

MOV SI, HelloString     ;Store string pointer to SI
CALL PrintString        ;Call print string procedure
JMP $                   ;Infinite loop 