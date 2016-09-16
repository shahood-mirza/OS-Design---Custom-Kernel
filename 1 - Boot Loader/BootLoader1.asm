[BITS 16]
[ORG 0x7C00] 

CALL PrintCharacter 
JMP $

PrintCharacter:
MOV AL, 65 
MOV AH, 0x0E
MOV BH, 0x00 
INT 0x10
RET

TIMES 510 - ($ - $$) db 0 
DW 0xAA55 
