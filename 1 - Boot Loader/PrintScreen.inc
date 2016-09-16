%ifndef _PRINTSCREEN_INC_
%define _PRINTSCREEN_INC_

;************************************************;
;	PrintCharacter
;	Prints a single character
;	AL: the character
;************************************************;

PrintCharacter:		
			
MOV AH, 0x0E		;Tell BIOS that we need to print one character on screen.
MOV BH, 0x00		;Page no.

INT 0x10			;Call video interrupt
RET			;Return to calling procedure

;************************************************;
;	PrintString
;	Prints a null terminated string
;	DS=>SI: 0 terminated string
;************************************************;

PrintString:		
;Your lab #1 code here
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
next_character:
MOV AL, [SI]
INC SI
OR AL, AL
JZ exit_function
CALL PrintCharacter
JMP next_character
exit_function:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
RET			;Return from procedure

%endif ;_PRINTSCREEN_INC_
