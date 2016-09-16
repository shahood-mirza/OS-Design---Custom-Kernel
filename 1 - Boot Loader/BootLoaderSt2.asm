[BITS 16]
[ORG 0x0000]

jmp main

;Included functions
%include "PrintScreen.inc"

;Data
HelloString db 'Hello World',10,13,0

main:
MOV AX, CS
MOV DS, AX
MOV ES, AX

MOV SI, HelloString 	
CALL PrintString	
JMP $ 	
