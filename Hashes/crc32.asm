section .text
		global crc32allAsm

;========================================================
;========================================================
;--------------------------------------------------------
; TableFind
;--------------------------------------------------------
; Entry: 
; Exit :
; Destr: 
;--------------------------------------------------------    	
crc32allAsm:			
	xor 		rax, 		rax		
	
	%assign 	offset 		1
	%rep 		4

	crc32 		rax, 		QWORD [rdi + offset]

	%assign  	offset		offset + 8
	%endrep

	xor 		rax,  		-1

	ret

