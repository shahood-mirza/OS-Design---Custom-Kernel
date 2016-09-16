[BITS 16]       ;16-bits real mode
[ORG 0x7C00]    ;Origin, tell the assembler that where the code will be in memory after it is been loaded

jmp main

;Included functions
%include "PrintScreen.inc"

;Data
LoadString db 'Stage 1 Bootloader Starting...', 10, 13, 0       
DriveNumber db 0x00


main:
xor AX, AX
MOV DS, AX
MOV ES, AX
mov ss, ax
mov sp, 7C00h

mov [DriveNumber], dl

MOV SI, LoadString      ;Store string pointer to SI
CALL PrintString        ;Call print string procedure


    reset:                      ; Reset the floppy drive
            mov ax, 0           ;
            ;mov dl, 80h           ; Drive: 0=first floppy, 80h = first HDD
            mov dl, byte [DriveNumber] ; drive
            int 13h             ;
            jc reset            ; ERROR => reset again


    read:
            mov ax, 1000h       ; ES:BX = 1000:0000
            mov es, ax          ;
            mov bx, 0           ;

            mov ah, 2           ; Load disk data to ES:BX
            mov al, 1           ; Load 1 sectors
            mov ch, 0           ; Cylinder=0
            mov cl, 2           ; Sector=2
            mov dh, 0           ; Head=0
            ;mov dl, 80h         ; Drive
            mov dl, byte [DriveNumber] ; drive
            int 13h             ; Read!

            jc read             ; ERROR => Try again


            jmp 1000h:0000      ; Jump to the program



TIMES 510 - ($ - $$) db 0       ;Fill the rest of sector with 0
DW 0xAA55                       ;Add boot signature at the end of bootloader 