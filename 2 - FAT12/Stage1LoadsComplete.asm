[BITS 16]	;16-bits real mode
[ORG 0x0]	;Origin will be set manually later on

jmp main

;*********************************************
;	Included function libraries
;*********************************************
%include "Fat12.inc"
%include "PrintScreen.inc"

;*********************************************
;	Program data
;*********************************************


; copy root dir above bootcode (7C00:0200)
ROOT_OFFSET  dw 0x0200	
ROOT_SEGMENT dw 0x07C0

; copy FAT above bootcode, over root dir (7C00:0200)
FAT_OFFSET dw 0x0200	
FAT_SEGMENT dw 0x07C0

; Stage 2 bootloader loaded to 0050:0000
ImageName     db "STAGE2  SYS"
IMAGE_SEGMENT dw 0x0050
IMAGE_OFFSET  dw 0x0000     

;Root offset
FIRST_ENTRY dw 0x0


;*********************************************
;	Bootloader Entry Point
;*********************************************

main:

     ;----------------------------------------------------
     ; code located at 0000:7C00, adjust segment registers
     ;----------------------------------------------------
     
          cli						; disable interrupts
          mov     ax, 0x07C0				; setup registers to point to our segment
          mov     ds, ax
          mov     es, ax
          mov     fs, ax
          mov     gs, ax

     ;----------------------------------------------------
     ; create stack
     ;----------------------------------------------------
     
          mov     ax, 0x0000				; set the stack
          mov     ss, ax
          mov     sp, 0xFFFF
          sti						; restore interrupts

     ;----------------------------------------------------
     ; Display loading message
     ;----------------------------------------------------
     
          
     ;----------------------------------------------------
     ; Load root directory table
     ;----------------------------------------------------
     CALL LOAD_ROOT

     ;----------------------------------------------------
     ; Find stage 2
     ;----------------------------------------------------

	CALL FindFile
	cmp	ax, -1
	jne	LOAD_FILE
	jmp     FAILURE

     ;----------------------------------------------------
     ; Load FAT
     ;----------------------------------------------------

     LOAD_FILE:
     ; read image file into memory (image-segment:image-offset)
     

	  CALL LoadFile
          push    WORD [IMAGE_SEGMENT]
          push    WORD [IMAGE_OFFSET]
          retf
          
          
     FAILURE:
     
	cli
	hlt
          ;mov     ah, 0x00
          ;int     0x16                                ; await keypress
          ;int     0x19                                ; warm boot computer
     
     
          TIMES 510-($-$$) DB 0
          DW 0xAA55
