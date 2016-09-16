
;*******************************************************
;
;	Stage2.asm
;		Stage2 Bootloader
;
;*******************************************************

bits	16

org 0x500				; where this bootloader is loaded

jmp	main				; go to start

;*******************************************************
;	Preprocessor directives
;*******************************************************

%include "Fat12.inc"
%include "PrintScreen.inc"		; INT 10h display functions
%include "Gdt.inc"			; GDT functions
%include "A20.inc"			; A20 enabling functions

;*******************************************************
;	Data Section
;*******************************************************

; where the kernel is to be loaded to in protected mode
%define IMAGE_PMODE_BASE 0x100000

; where the kernel is to be loaded to in real mode
%define IMAGE_RMODE_BASE 0x3000

; kernel name (Must be 11 bytes)
ImageName     db "KERNEL  SYS"
IMAGE_SEGMENT dw 0x0000
IMAGE_OFFSET  dw 0x3000     

; size of kernel image in bytes
ImageSize     db 0

LoadingMsg db 0x0D, 0x0A, "Searching for Operating System...", 0x00
msgFailure db 0x0D, 0x0A, "*** FATAL: MISSING OR CURRUPT KRNL.SYS. Press Any Key to Reboot", 0x0D, 0x0A, 0x0A, 0x00

; copy root dir at 02e0:0000
ROOT_OFFSET  dw 0x0	
ROOT_SEGMENT dw 0x2e0

; copy FAT at (2e00:0000)
FAT_OFFSET dw 0x0	
FAT_SEGMENT dw 0x2c0

;Root offset
FIRST_ENTRY dw 0x2e00


;*******************************************************
;	STAGE 2 ENTRY POINT
;
;		-Install GDT
;		-Enable gate A20
;		-Load kernel
; 		-Go into protected mode (pmode)
;		-Jump to Protected Mode
;		-Copy kernel to 1MB
;		-Jump to kernel
;*******************************************************

main:

	;-------------------------------;
	;   Setup segments and stack	;
	;-------------------------------;

	cli				; clear interrupts
	xor 	ax, ax			; null segments
	mov	ds, ax
	mov	es, ax
	mov	ax, 0x0			; stack begins at 0x9000-0xffff
	mov	ss, ax
	mov	sp, 0xFFFF
	sti				; enable interrupts

	;-------------------------------;
	;   Install our GDT		;
	;-------------------------------;

	call	InstallGDT		; install our GDT

	;-------------------------------;
	;   Enable A20			;
	;-------------------------------;

	call	EnableA20_KKbrd_Out

	;-------------------------------;
	;   Print loading message	;
	;-------------------------------;

	mov	si, LoadingMsg
	call	PrintString

        ;-------------------------------;
        ; Initialize filesystem		;
        ;-------------------------------;

	call	LOAD_ROOT		; Load root directory table

        ;-------------------------------;
        ; Load Kernel			;
        ;-------------------------------;

	call	FindFile		
	cmp	ax, -1
	jne	LOAD_FILE
	JMP	FAILURE

LOAD_FILE:
	sub	edi, DWORD [FIRST_ENTRY]
	sub	eax, DWORD [FIRST_ENTRY]

	call LoadFile
	mov	dword [ImageSize], ecx	; save size of kernel
	jmp	EnterPMode		; nto Protected Mode!
FAILURE:
	pop	bp
	pop	bx
	pop	ecx
	mov	si, msgFailure		; Nope--print error
	call	PrintString
	mov	ah, 0
	int     0x16                    ; Await keyboard input (i.e. "press any key")
	int     0x19                    ; Load and execute stage-1 bootloader
	cli				; It should not be possible to get here
	hlt

	;-------------------------------;
	;   Go into pmode		;
	;-------------------------------;

EnterPMode:

	cli				; clear interrupts
	mov	eax, cr0		; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax

	jmp	CODE_DESC:PMode		; far jump to kernel-mode code descriptor

	; Note: Do NOT re-enable interrupts! Doing so will triple fault!

;******************************************************
;	Protected Mode!
;******************************************************

bits 32

PMode:

	;-------------------------------;
	;   Set registers		;
	;-------------------------------;

	mov	ax, DATA_DESC		; set data segments to kernel-mode data descriptor
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 90000h		; stack begins from 90000h

	;-------------------------------;
	; Copy kernel to 1MB		;
	;-------------------------------;

CopyImage:
  	 mov	eax, dword [ImageSize]
  	 movzx	ebx, word [bpbBytesPerSector]
  	 mul	ebx
  	 mov	ebx, 4
  	 div	ebx
   	 cld
   	 mov    esi, IMAGE_RMODE_BASE
   	 mov	edi, IMAGE_PMODE_BASE
   	 mov	ecx, eax
   	 rep	movsd                   ; copy image to its protected mode address

	;---------------------------------------;
	;   Execute Kernel			;
	;---------------------------------------;

	jmp	CODE_DESC:IMAGE_PMODE_BASE	; jump to our kernel! 

	;---------------------------------------;
	;   Stop execution			;
	;---------------------------------------;

	cli
	hlt

