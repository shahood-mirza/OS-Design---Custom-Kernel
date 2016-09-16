[BITS 16]	;16-bits real mode
[ORG 0x7C00]	;Origin, tell the assembler that where the code will be in memory after it is been loaded

jmp main

;*************************************************;
;	OEM Parameter block / BIOS Parameter Block
;*************************************************;

bpbOEM			DB "ENGI3655"
bpbBytesPerSector:  	DW 512
bpbSectorsPerCluster: 	DB 1
bpbReservedSectors: 	DW 1
bpbNumberOfFATs: 	DB 2
bpbRootEntries: 	DW 224
bpbTotalSectors: 	DW 2880
bpbMedia: 	        DB 0xF0
bpbSectorsPerFAT: 	DW 9
bpbSectorsPerTrack: 	DW 18
bpbHeadsPerCylinder: 	DW 2
bpbHiddenSectors:       DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
bsUnused: 	        DB 0
bsExtBootSignature: 	DB 0x29
bsSerialNumber:	        DD 0xa0a1a2a3
bsVolumeLabel: 	        DB "MOS FLOPPY "
bsFileSystem: 	        DB "FAT12   "

;Included functions
%include "PrintScreen.inc"

;Data
LoadString db 'Stage 1 Bootloader Starting...', 10, 13, 0	;HelloWorld string ending with 0


main:
MOV AX, CS
MOV DS, AX
MOV ES, AX

MOV SI, LoadString 	;Store string pointer to SI
CALL PrintString	;Call print string procedure


    reset:                      ; Reset the floppy drive
            mov ax, 0           ;
            mov dl, 0           ; Drive=0 (=A)
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
            mov dl, 0           ; Drive=0
            int 13h             ; Read!

            jc read             ; ERROR => Try again


            jmp 1000h:0000      ; Jump to the program



TIMES 510 - ($ - $$) db 0	;Fill the rest of sector with 0
DW 0xAA55			;Add boot signature at the end of bootloader
