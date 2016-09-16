
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
	dd 0                ; null descriptor
	dd 0 

; gdt code:	            ; code descriptor
	dw 0FFFFh           ; limit low
	dw 0                ; base low
	db 0                ; base middle
	db 10011010b        ; access
	db 11001111b        ; granularity
	db 0                ; base high

; gdt data:	            ; data descriptor
	dw 0FFFFh           ; limit low (Same as code)
	dw 0                ; base low
	db 0                ; base middle
	db 10010010b        ; access
	db 11001111b        ; granularity
	db 0                ; base high

; gdt user code:            ; code descriptor
	dw 0FFFFh           ; limit low
	dw 0                ; base low
	db 0                ; base middle
	db 11111010b        ; access
	db 11001111b        ; granularity
	db 0                ; base high

; gdt user data:            ; data descriptor
	dw 0FFFFh           ; limit low (Same as code)
	dw 0                ; base low
	db 0                ; base middle
	db 11110010b        ; access
	db 11001111b        ; granularity
	db 0                ; base high
	
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




