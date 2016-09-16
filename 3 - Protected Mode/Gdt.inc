
%ifndef __GDT_INC__
%define __GDT_INC__

bits	16

;*******************************************
; Install our GDT
;*******************************************

InstallGDT:

	cli                  ; clear interrupts
	pusha                ; save registers
	lgdt 	[toc]        ; load GDT into GDTR
	sti	             ; enable interrupts
	popa                 ; restore registers
	ret	             ; All done!

;*******************************************
; Global Descriptor Table (GDT)
;*******************************************

gdt_data: 

; null descriptor
	dd 0
	dd 0

; kernel code descriptor
	dw 0FFFFh
	dw 0
	db 0
	db 10011010b
	db 11001111b
	db 0

; kernel data descriptor
	dw 0FFFFh
	dw 0
	db 0
	db 10010010b
	db 11001111b
	db 0
	
end_of_gdt:
toc: 
	dw end_of_gdt - gdt_data - 1 	; limit (Size of GDT)
	dd gdt_data 			; base of GDT

; give the descriptor offsets names

%define NULL_DESC 0
%define CODE_DESC 0x8
%define DATA_DESC 0x10
%define USER_CODE_DESC 0x18
%define USER_DATA_DESC 0x20

%endif ;__GDT_INC__




