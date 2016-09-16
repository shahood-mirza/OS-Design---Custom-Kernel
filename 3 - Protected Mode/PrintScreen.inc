%ifndef _PRINTSCREEN_INC_
%define _PRINTSCREEN_INC_

;************************************************;
;	PrintCharacter
;	Prints a single character
;	AL: the character
;************************************************;


PrintCharacter:		;Procedure to print character on screen
			;Assume that ASCII value is in register AL
MOV AH, 0x0E		;Tell BIOS that we need to print one charater on screen.
MOV BH, 0x00		;Page no.

INT 0x10		;Call video interrupt
RET			;Return to calling procedure


;************************************************;
;	PrintString
;	Prints a null terminated string
;	DS=>SI: 0 terminated string
;************************************************;


PrintString:		;Procedure to print string on screen
			;Assume that string starting pointer is in register SI

next_character:		;Lable to fetch next character from string
LODSB 
OR AL, AL		;Check if value in AL is zero (end of string)
JZ exit_function 	;If end then return
CALL PrintCharacter 	;Else print the character which is in AL register
JMP next_character	;Fetch next character from string
exit_function:		;End label
RET			;Return from procedure

%endif ;_PRINTSCREEN_INC_
