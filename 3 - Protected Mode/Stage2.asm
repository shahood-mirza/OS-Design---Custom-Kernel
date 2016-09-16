
bits	16

org 0x0

jmp	main				; go to start

;*******************************************************
;	Preprocessor directives
;*******************************************************

%include "PrintScreen.inc"		; Display functions
%include "Gdt.inc"			; GDT functions
%include "A20.inc"			; A20 memory functions

;*******************************************************
;	Data Section
;*******************************************************

Message1 db 0x0D, 0x0A, "Stage 2 Bootloader Starting...", 0x0D, 0x0A, 0x00
Message2 db 0x0D, 0x0A, "GDT Installed...", 0x0D, 0x0A, 0x00
Message3 db 0x0D, 0x0A, "Gate A20 Enabled...", 0x0D, 0x0A, 0x00

;*******************************************************
;	STAGE 2 ENTRY POINT
;*******************************************************

main:

	;-------------------------------;
	;   Setup segments and stack	;
	;-------------------------------;

	cli				; clear interrupts
	mov 	ax, cs			
	mov	ds, ax
	mov	es, ax
	mov	ax, 0x9000		; stack begins at 0x9000-0xffff
	mov	ss, ax
	mov	sp, 0xFFFF
	sti				; enable interrupts

	;-------------------------------;
	;   Print loading message	;
	;-------------------------------;

	mov	si, Message1
	call	PrintString

	;-------------------------------;
	;   Install our GDT		;
	;-------------------------------;

	call	InstallGDT		; install our GDT
	mov	si, Message2
	call	PrintString

	;-------------------------------;
	;   Enable A20			;
	;-------------------------------;

	call	EnableA20_KKbrd_Out
	mov	si, Message3
	call	PrintString

	;-------------------------------;
	;   Go into pmode		;
	;-------------------------------;

	cli				; clear interrupts
	mov	eax, cr0		; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax


	jmp 			$	; Infinite loop