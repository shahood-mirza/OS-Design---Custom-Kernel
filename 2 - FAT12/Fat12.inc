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
          sub     ax, 0x0002                          ; subtract 2 from ax (cluster of base zero)
          xor     cx, cx
          mov     cl, BYTE [bpbSectorsPerCluster]     ; converting the byte to a word
          mul     cx				      ; unsigned multiply by value in cx
          add     ax, WORD [datasector]               ; add to ax (data sector)
          ret
     
;************************************************;
; Convert logical sector number to head/track/sector number
;************************************************;

LBACHS:
          xor     dx, dx  
          div     WORD [bpbSectorsPerTrack]           ; divide dx by number of Sectors Per Track
          inc     dl                                  ; increment dl by one
          mov     BYTE [absoluteSector], dl
          xor     dx, dx                   
          div     WORD [bpbHeadsPerCylinder]          ; divide dx by number of Heads Per Cylinder
          mov     BYTE [absoluteHead], dl	      ; store value into dl
          mov     BYTE [absoluteTrack], al	      ; store value into al
          ret

;************************************************;
; Load root directory table
; Loaded to ROOT_SEGMENT:ROOT_OFFSET
;************************************************;

LOAD_ROOT:

     ; store registers     
          pusha							
          push	es

     ; calculating the size of the root and storing into cx
     
          xor     cx, cx
          xor     dx, dx
          mov     ax, 0x0020                           ; move 32 into ax (32 byte directory)
          mul     WORD [bpbRootEntries]                ; total directory size
          div     WORD [bpbBytesPerSector]             ; number of sectors used by the directory
          xchg    ax, cx			       ; exchange value from ax and cx (directory size)
          
     ; finding the location of the root directory and storing into ax
     
          mov     al, BYTE [bpbNumberOfFATs]           ; moving the number of FATs into al
          mul     WORD [bpbSectorsPerFAT]              ; multiply by number of sectors used by FATs
          add     ax, WORD [bpbReservedSectors]        ; we need to take the reserved sectors into consideration (ie bootsector)
 
     ; storring sector locations

          mov     WORD [datasector], ax                ; base of root directory into ax
          add     WORD [datasector], cx                ; adding to cx
          
     ; read the directory
     
	  mov 	  es, WORD [ROOT_SEGMENT]	       ; Segment is 7C00
          mov     bx, WORD [ROOT_OFFSET]	       ; Offset is 0200
          call    ReadSectors    		       ; read sectors from directory table

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

     ; calculating the size of FAT and storing into cx
     
          xor     ax, ax
          mov     al, BYTE [bpbNumberOfFATs]          	; moving number of FATs into al
          mul     WORD [bpbSectorsPerFAT]             	; multiply by the number of sectors used by FATs
          mov     cx, ax				; move value into cx from ax

     ; calculate where FAT is located and store into ax

          mov     ax, WORD [bpbReservedSectors]       	; compute start of FAT (right after reserved sectors)
          
     ; read FAT into memory

	  mov 	  es, WORD [FAT_SEGMENT]		; Segment is 7C00
          mov     bx, WORD [FAT_OFFSET]			; Offset is 0200
          call    ReadSectors				; read in ReadSectors cluster
          

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

     ; browse root directory table for binary image

	mov     cx, WORD [bpbRootEntries]			; move number of Root Entries into cx (for loop)
	mov     di, WORD [FIRST_ENTRY]				; finding first root entry
	cld							

.LOOP:
	push    cx
	mov     cx, 0x000B				        ; store 11 into cx (eleven character filename)
	mov	si, ImageName					; move ImageName into si
 	push    di
	rep  cmpsb						; matching each entry if possible
	pop     di
	je      .Found						; if equal, go to .Found
	pop     cx
	add     di, 0x0020				        ; add 32 to di (next directory entry)
	loop    .LOOP

.NotFound:
	mov	ax, -1						; error incase nothing is found
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
	mov 	es, WORD [ROOT_SEGMENT] 	; root segment location
	mov	dx, WORD [es:di + 0x001A]	; point to the file entry in the RDT
	mov	WORD [cluster], dx		; first cluster in the file
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

	; calculate the next cluster

	mov     ax, WORD [cluster]	; current cluster, moved into ax
	mov     cx, ax			
	mov     dx, ax
	shr     dx, 0x0001		; divide by two
	add     cx, dx			; sum for (3/2)

	push	bx
	push	es
	mov	es, WORD [FAT_SEGMENT]	; next segment location
	mov	bx, WORD [FAT_OFFSET]
	add	bx, cx
	mov	dx, WORD [es:bx]

	test	ax, 0x0001		; check if the address is odd
	jnz	.ODD_CLUSTER

.EVEN_CLUSTER:

	and	dx, 0000111111111111b	; use the lower twelve bits if even
	jmp	.DONE

.ODD_CLUSTER:

	shr	dx, 0x0004		; use upper twelve bits if odd

.DONE:

	mov	WORD [cluster], dx
	cmp	dx, 0x0ff0		; check if it is the end-of-file cluster
	jb	.LOAD_IMAGE		; if not keep loading clusters

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





