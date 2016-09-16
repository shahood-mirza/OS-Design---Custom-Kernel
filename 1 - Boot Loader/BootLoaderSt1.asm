[BITS 16]	
[ORG 0x7C00]

jmp main

;Included functions
%include "PrintScreen.inc"
;Data
LoadString db 'Stage 1 Bootloader Starting...', 10, 13, 0

main:
MOV AX, CS
MOV DS, AX
MOV ES, AX

MOV SI, LoadString
CALL PrintString	

reset: 
MOV AX, 0
MOV DL, 0
int 13h
JC reset

read:
MOV AX, 1000h
MOV ES, ax
MOV BX, 0

MOV AH, 2
MOV AL, 1
MOV CH, 0
MOV CL, 2
MOV DH, 0
MOV DL, 0
INT 13h

JC read


JMP 1000h:0000

TIMES 510 - ($ - $$) db 0
DW 0xAA55
