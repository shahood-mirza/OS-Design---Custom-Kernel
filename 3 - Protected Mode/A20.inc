
%ifndef __A20_INC__
%define __A20_INC__

bits	16

;--------------------------------------------
; Enables a20 line through output port
;--------------------------------------------

EnableA20_KKbrd_Out:

	cli
	pusha
	
 	call    wait_input	; wait for input buffer to clear
        mov     al,0xAD
        out     0x64,al		; disable keyboard
        call    wait_input	; wait for input buffer to clear

	mov     al,0xD0
        out     0x64,al		; read output port
        call    wait_output	; wait for output buffer to clear

	in      al,0x60
        push    eax		; get and store data from output port (pushed onto stack)
        call    wait_input	; wait for input buffer to clear

	mov     al,0xD1
        out     0x64,al		; get controller to write back to output port
        call    wait_input	; wait for input buffer to clear

	pop     eax
        or      al,2		; set bit 1 (enable a20)
        out     0x60,al

	call    wait_input	; wait for input buffer to clear
        mov     al,0xAE		; enable keyboard
        out     0x64,al

        call    wait_input	; wait for input buffer to clear

	popa
        sti
        ret

; custom function to wait for input buffer to get cleared

wait_input:
        in      al,0x64
        test    al,2
        jnz     wait_input
        ret

; custom function to wait for output buffer to get cleared

wait_output:
        in      al,0x64
        test    al,1
        jz      wait_output
        ret


%endif ;__A20_INC__
