%ifndef _FAT12_INC_
%define _FAT12_INC_

;*********************************************
;	BIOS Parameter Block
;*********************************************

bpbOEM			db "ENGI3655"
bpbBytesPerSector:  	DW 512
bpbSectorsPerCluster: 	DB 1
bpbReservedSectors: 	DW 1
bpbNumberOfFATs: 	DB 2
bpbRootEntries: 	DW 224
bpbTotalSectors: 	DW 2880
bpbMedia: 		DB 0xf0
bpbSectorsPerFAT: 	DW 9
bpbSectorsPerTrack: 	DW 18
bpbHeadsPerCylinder: 	DW 2
bpbHiddenSectors: 	DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
bsUnused: 		DB 0
bsExtBootSignature: 	DB 0x29
bsSerialNumber:	        DD 0xa0a1a2a3
bsVolumeLabel: 	        DB "OUR FLOPPY "
bsFileSystem: 	        DB "FAT12   "

;************************************************;
; Reads a series of sectors
; CX=>Number of sectors to read
; AX=>Starting sector
; ES:BX=>Buffer to read to
;************************************************;

ReadSectors:
     .MAIN:
          mov     di, 0x0005                          ; five retries for error
     .SECTORLOOP:
          push    ax
          push    bx
          push    cx
          call    LBACHS                              ; convert starting sector to CHS
          mov     ah, 0x02                            ; BIOS read sector
          mov     al, 0x01                            ; read one sector
          mov     ch, BYTE [absoluteTrack]            ; track
          mov     cl, BYTE [absoluteSector]           ; sector
          mov     dh, BYTE [absoluteHead]             ; head
          mov     dl, BYTE [bsDriveNumber]            ; drive
          int     0x13                                ; invoke BIOS
          jnc     .SUCCESS                            ; test for read error
          xor     ax, ax                              ; BIOS reset disk
          int     0x13                                ; invoke BIOS
          dec     di                                  ; decrement error counter
          pop     cx
          pop     bx
          pop     ax
          jnz     .SECTORLOOP                         ; attempt to read again
          int     0x18
     .SUCCESS:
          mov     si, msgProgress
          call    PrintString
          pop     cx
          pop     bx
          pop     ax
          add     bx, WORD [bpbBytesPerSector]        ; queue next buffer
          inc     ax                                  ; queue next sector
          loop    .MAIN                               ; read next sector
          ret

;************************************************;
; Convert CHS to LBA
; LBA = (cluster - 2) * sectors per cluster
;************************************************;

ClusterLBA:
          sub     ax, 0x0002                          ; zero base cluster number
          xor     cx, cx
          mov     cl, BYTE [bpbSectorsPerCluster]     ; convert byte to word
          mul     cx
          add     ax, WORD [datasector]               ; base data sector
          ret
     
;************************************************;
; Convert LBA to CHS
; AX=>LBA Address to convert
;
; absolute sector = (logical sector / sectors per track) + 1
; absolute head   = (logical sector / sectors per track) MOD number of heads
; absolute track  = logical sector / (sectors per track * number of heads)
;
;************************************************;

LBACHS:
          xor     dx, dx                              ; prepare dx:ax for operation
          div     WORD [bpbSectorsPerTrack]           ; calculate
          inc     dl                                  ; adjust for sector 0
          mov     BYTE [absoluteSector], dl
          xor     dx, dx                              ; prepare dx:ax for operation
          div     WORD [bpbHeadsPerCylinder]          ; calculate
          mov     BYTE [absoluteHead], dl
          mov     BYTE [absoluteTrack], al
          ret

;************************************************;
; Load root directory table
; Loaded to ROOT_SEGMENT:ROOT_OFFSET
;************************************************;

LOAD_ROOT:

     ; store registers     
          pusha							
          push	es

     ; compute size of root directory and store in "cx"
     
          xor     cx, cx
          xor     dx, dx
          mov     ax, 0x0020                           ; 32 byte directory entry
          mul     WORD [bpbRootEntries]                ; total size of directory
          div     WORD [bpbBytesPerSector]             ; sectors used by directory
          xchg    ax, cx
          
     ; compute location of root directory and store in "ax"
     
          mov     al, BYTE [bpbNumberOfFATs]            ; number of FATs
          mul     WORD [bpbSectorsPerFAT]               ; sectors used by FATs
          add     ax, WORD [bpbReservedSectors]         ; adjust for bootsector
          mov     WORD [datasector], ax                 ; base of root directory
          add     WORD [datasector], cx
          
     ; read root directory into memory
     
	  mov 	  es, WORD [ROOT_SEGMENT]
          mov     bx, WORD [ROOT_OFFSET]
          call    ReadSectors    

     ; restore registers and return
	  pop     es
	  popa
	  ret


;*******************************************
; LOAD_FAT 
;	Loads FAT table
;       Loaded to FAT_SEGMENT:FAT_OFFSET
;
;	Parm/ ES:DI => Root Directory Table
;*******************************************

LOAD_FAT:
     ; store registers
          pusha
          push	es

     ; compute size of FAT and store in "cx"
     
          xor     ax, ax
          mov     al, BYTE [bpbNumberOfFATs]          ; number of FATs
          mul     WORD [bpbSectorsPerFAT]             ; sectors used by FATs
          mov     cx, ax

     ; compute location of FAT and store in "ax"

          mov     ax, WORD [bpbReservedSectors]       ; adjust for bootsector
          
     ; read FAT into memory (7C00:0200)

	  mov 	  es, WORD [FAT_SEGMENT]
          mov     bx, WORD [FAT_OFFSET]
          call    ReadSectors
          

     ; restore registers and return
          pop  	  es
          popa
          ret

;*******************************************
; FindFile ()
;	- Search for filename in root table
;
; parm/ ImageName File name
; ret/ AX => File index number in directory table. -1 if error
;*******************************************

FindFile:

	push	cx						; store registers
	push	dx
	push	bx

     ; browse root directory for binary image

	mov     cx, WORD [bpbRootEntries]			; load loop counter
	mov     di, WORD [FIRST_ENTRY]				; locate first root entry at 1 MB mark
	cld							; clear direction flag
	
.LOOP:
	push    cx
	mov     cx, 0x000B				        ; eleven character name. Image name is in SI
	mov	si, ImageName					
 	push    di
     rep  cmpsb							; test for entry match
	pop     di
	je      .Found
	pop     cx
	add     di, 0x0020				        ; queue next directory entry
	loop    .LOOP

.NotFound:
	mov	ax, -1						; set error code
	jmp	.Exit

.Found:
	pop	ax						; return value into AX contains entry of file
.Exit:
	pop	bx						; restore registers and return
	pop	dx
	pop	cx
	ret


;*******************************************
; LoadFile ()
;	- Load file
; parm/ ES:SI => File to load
; parm/ IMAGE_SEGMENT:IMAGE_OFFSET => Buffer to load file to
; ret/ CX => number of sectors read
;*******************************************


LoadFile:
	mov     es, WORD [IMAGE_SEGMENT]
        mov     bx, WORD [IMAGE_OFFSET]

	xor	ecx, ecx		; size of file in sectors
	push	ecx
	push	bx			; BX=>BP points to buffer to write to; store it for later
	;push	bp


	; get starting cluster

	push 	es
	mov 	es, WORD [ROOT_SEGMENT] ;Root segment location
	mov	dx, WORD [es:di + 0x001A]; DI points to file entry in root directory table. Refrence the table...
	mov	WORD [cluster], dx	; file's first cluster
	pop 	es
	pop	bx			; get location to write to so we dont screw up the stack
	push    bx			; store location for later again
	push	es

	call	LOAD_FAT


.LOAD_IMAGE:

	; load the cluster

	mov	ax, WORD [cluster]	; cluster to read
	pop	es			; bx:bp=es:bx
	pop	bx
	call	ClusterLBA
	xor	cx, cx
	mov     cl, BYTE [bpbSectorsPerCluster]
	call	ReadSectors
	pop	ecx
	inc	ecx			; add one more sector to counter
	push	ecx

	; get next cluster

	mov     ax, WORD [cluster]	; identify current cluster
	mov     cx, ax			; copy current cluster
	mov     dx, ax
	shr     dx, 0x0001		; divide by two
	add     cx, dx			; sum for (3/2)

	push	bx
	push	es
	mov	es, WORD [FAT_SEGMENT]	;location of fat in memory
	mov	bx, WORD [FAT_OFFSET]
	add	bx, cx
	mov	dx, WORD [es:bx]
	test	ax, 0x0001		; test for odd or even cluster
	jnz	.ODD_CLUSTER

.EVEN_CLUSTER:

	and	dx, 0000111111111111b	; take low 12 bits
	jmp	.DONE

.ODD_CLUSTER:

	shr	dx, 0x0004		; take high 12 bits

.DONE:

	mov	WORD [cluster], dx
	cmp	dx, 0x0ff0		; test for end of file marker
	jb	.LOAD_IMAGE

;SUCCESS:
	pop	es
	pop	bx
	pop	ecx
	ret


;*******************************************
; Variables and messages
;*******************************************

     absoluteSector db 0x00
     absoluteHead   db 0x00
     absoluteTrack  db 0x00
     
     datasector  dw 0x0000
     cluster     dw 0x0000

     msgCRLF     db "X", 0x00
     msgProgress db ".", 0x00



%endif		;_FAT12_INC_





