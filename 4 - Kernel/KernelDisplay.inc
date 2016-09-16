
%ifndef __KernelDisplay_INC__
%define __KernelDisplay_INC__


;==========================================================
;
;	 32 Bit Protected Mode Routines
;==========================================================

bits 32

; The constants and variables defined here can be useful in your functions!

%define		VIDMEM	0xB8000			; video memory
%define		COLS	80			; width and height of screen
%define		LINES	25
%define		CHAR_ATTRIB 63			; character attribute (White text on cyan background)

_CurX db 0					; current x/y location
_CurY db 0

;**************************************************;
;	DisplayChar
;	Prints a character to screen
;
;	Input assumes:
;	BL 	=> Character to print
;	_CurX 	=> Horizontal screen position to write to
;	_CurY 	=> Vertical screen position to write to
;	VIDMEM, COLS, CHAR_ATTRIB
;
;	Output:
;	Updated values of _CurX and _CurY
;**************************************************;

DisplayChar:

	pusha				; Save registers
	push edi

	;-------------------------------;
	;1. Compute memory location (in bytes) and store in edi
	;
	; (X,Y) position on screen = 
	;	Memory start + Y * 2 * screen width in characters + X * 2
	;-------------------------------;
	
	xor ecx, ecx
	mov ecx, COLS*2
	xor eax, eax
	mov al, [_CurY]
	mul ecx
	push eax
	mov al, [_CurX]
	mov cl, 2
	mul cl
	pop ecx
	add eax, ecx
	add eax, VIDMEM
	mov edi, eax

	;-------------------------------;
	;2. Changes if the character to display is a new line symbol
	;
	;-------------------------------;
	
	cmp bl, 0x0A
	jne .continue
	cmp BYTE [_CurY], LINES
	je .break
	inc BYTE [_CurY]
	.break:
	jmp .done
	.continue:

	;-------------------------------;
	;3. Display the character with attributes
	;
	;-------------------------------;
	
	mov byte [edi], bl
	mov byte [edi+1], CHAR_ATTRIB

	;-------------------------------;
	;4. Increment the position
	;So the next character will be after the one we just wrote
	;
	;-------------------------------;

	cmp BYTE [_CurX], COLS
	je .incy
	inc BYTE [_CurX]
	jmp .done
	.incy:
	cmp BYTE [_CurY], LINES
	je .done
	mov byte [_CurX], 0
	inc BYTE [_CurY]


.done:
	pop edi
	popa				; Restore registers 
	ret				; Return

;**************************************************;
;	DisplayString
;	Prints a null terminated string
;
;	EBX => address of string to print
;**************************************************;

DisplayString:

	pusha				; Save registers
	push	ebx			; Copy the string address...
	pop	edi			; ...to ES:DI

.loop:

	;-------------------------------;
	;   Get character...            ;
	;-------------------------------;

	mov bl, byte [edi] ; get next character
	cmp bl, 0 ; compare with 0 (null terminator check)
	je .done ; if zero then exit

	;-------------------------------;
	;   Print the character...      ;
	;-------------------------------;
	
	call DisplayChar ; if not zero then display
		
	;-------------------------------;
	;   Go to next character...     ;
	;-------------------------------;
	
	inc edi ; point to next character
	jmp .loop

.done:

	;-------------------------------;
	;   Update hardware cursor...   ;
	;-------------------------------;

	mov bh, byte [_CurY] ; get current position
	mov bl, byte [_CurX]
	call MoveCursor ; move cursor to current position

	popa				; Restore registers
	ret				; Return

;**************************************************;
;	MoveCur 
;	Update hardware cursor
;
;	Input assumes:
;	bh => Y pos
;	bl => x pos
;
;	Output:
;	None.
;**************************************************;

bits 32

MoveCursor:

	pusha				; Save registers 

	;-------------------------------;
	;1. Compute cursor location
	;After this, the cursor position will be a 2-byte address in the low part of EBX
	;
	;  Cursor position = Y * screen width in characters + X
	;-------------------------------;

	xor	eax, eax
	mov	ecx, COLS
	mov	al, bh	
	mul	ecx	
	add	al, bl
	mov	ebx, eax

	;--------------------------------------;
	;2. Update cursor location, low byte (BL)
	;
	;2a) Point to the right offset in the index register 
	;2b) Send the new value to the data register 
	;--------------------------------------;

	mov	al, 0x0f
	mov	dx, 0x03D4
	out	dx, al

	mov	al, bl
	mov	dx, 0x03D5
	out	dx, al			; low byte

	;---------------------------------------;
	;3. Update cursor location, high byte (BH)
	;
	;3a) Point to the right offset in the index register 
	;3b) Send the new value to the data register 
	;---------------------------------------;
	
	xor	eax, eax

	mov	al, 0x0e
	mov	dx, 0x03D4
	out	dx, al

	mov	al, bh
	mov	dx, 0x03D5
	out	dx, al			; high byte

	popa				; Restore registers
	ret				; Return

;**************************************************;
;	ClearScreen
;	Clears screen
;
;	Input assumes:
;	VIDMEM, CHAR_ATTRIB, COLS, LINES
;
;	Output:
;	Updated values of _CurX and _CurY
;**************************************************;

bits 32

ClearScreen:

	pusha				; Save registers 

	;-------------------------------;
	;1. Setup
	;a) Set copying order
	;b) EDI point to the start of video memory
	;c) CX contains the count value
	;d) AX contains a space character with the correct attributes
	;-------------------------------;
	
	cld
	mov edi, VIDMEM
	mov cx, COLS * LINES
	mov al, ' '
	mov ah, 00h

	;-------------------------------;
	;2. rep	stosw
	;-------------------------------;

	rep stosw

	;-------------------------------;
	;3. Set the current position to (0,0)
	;-------------------------------;

	mov byte [_CurX], 0
	mov byte [_CurY], 0

	popa				; Restore registers
	ret				; Return


%endif ;__KernelDisplay_INC__
