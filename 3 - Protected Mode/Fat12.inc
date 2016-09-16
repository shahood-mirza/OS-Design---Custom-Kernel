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
; Convert the cluster number to logical sector number
;************************************************;

ClusterLBA:
          sub     ax, 0x0002                          ; 
          xor     cx, cx
          mov     cl, BYTE [bpbSectorsPerCluster]     ; 
          mul     cx				      ;
          add     ax, WORD [datasector]               ; 
          ret
     
;************************************************;
; Convert logical sector number to head/track/sector number
;************************************************;

LBACHS:
          xor     dx, dx  
          div     WORD [bpbSectorsPerTrack]           ; 
          inc     dl                                  ; 
          mov     BYTE [absoluteSector], dl
          xor     dx, dx                   
          div     WORD [bpbHeadsPerCylinder]          ; 
          mov     BYTE [absoluteHead], dl	      ;
          mov     BYTE [absoluteTrack], al	      ;
          ret

;************************************************;
; Load root directory table
; Loaded to ROOT_SEGMENT:ROOT_OFFSET
;************************************************;

LOAD_ROOT:

     ; store registers     
          pusha							
          push	es

     ; 
     
          xor     cx, cx
          xor     dx, dx
          mov     ax, 0x0020                           ; 
          mul     WORD [bpbRootEntries]                ; 
          div     WORD [bpbBytesPerSector]             ; 
          xchg    ax, cx			       ;
          
     ; 
     
          mov     al, BYTE [bpbNumberOfFATs]           ; 
          mul     WORD [bpbSectorsPerFAT]              ; 
          add     ax, WORD [bpbReservedSectors]        ; 
 
     ; 

          mov     WORD [datasector], ax                ; 
          add     WORD [datasector], cx                ;
          
     ;
     
	  mov 	  es, WORD [ROOT_SEGMENT]	       ; Segment is
          mov     bx, WORD [ROOT_OFFSET]	       ; Offset is
          call    ReadSectors    		       ;

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

     ; 
     
          xor     ax, ax
          mov     al, BYTE [bpbNumberOfFATs]          	; 
          mul     WORD [bpbSectorsPerFAT]             	; 
          mov     cx, ax				;

     ; 

          mov     ax, WORD [bpbReservedSectors]       	; 
          
     ; 

	  mov 	  es, WORD [FAT_SEGMENT]		; Segment is
          mov     bx, WORD [FAT_OFFSET]			; Offset is
          call    ReadSectors				;
          

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

     ; store registers

	push	cx	
	push	dx
	push	bx

     ; browse root directory for binary image

	mov     cx, WORD [bpbRootEntries]			; 
	mov     di, WORD [FIRST_ENTRY]				; 
	cld							

.LOOP:
	push    cx
	mov     cx, 0x000B				        ; 
	mov	si, ImageName					;
 	push    di
     rep  cmpsb							; 
	pop     di
	je      .Found						;
	pop     cx
	add     di, 0x0020				        ; 
	loop    .LOOP

.NotFound:
	mov	ax, -1						; 
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

	xor	ecx, ecx
	push	ecx
	push	bx	


	; get starting cluster

	push 	es
	mov 	es, WORD [ROOT_SEGMENT] 	; 
	mov	dx, WORD [es:di + 0x001A]	; 
	mov	WORD [cluster], dx		; 
	pop 	es
	pop	bx				; get location to write to so we dont screw up the stack
	push    bx				; store location for later again
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

	; 

	mov     ax, WORD [cluster]	; 
	mov     cx, ax			
	mov     dx, ax
	shr     dx, 0x0001		; divide by two
	add     cx, dx			; sum for (3/2)

	push	bx
	push	es
	mov	es, WORD [FAT_SEGMENT]	; 
	mov	bx, WORD [FAT_OFFSET]
	add	bx, cx
	mov	dx, WORD [es:bx]

	test	ax, 0x0001		; 
	jnz	.ODD_CLUSTER

.EVEN_CLUSTER:

	and	dx, 0000111111111111b	; 
	jmp	.DONE

.ODD_CLUSTER:

	shr	dx, 0x0004		; 

.DONE:

	mov	WORD [cluster], dx
	cmp	dx, 0x0ff0		; 
	jb	.LOAD_IMAGE		; 

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

     ;msgCRLF     db 0x0D, 0x0A, 0x00
     msgProgress db ".", 0x00



%endif		;_FAT12_INC_





