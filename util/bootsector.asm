	 ;*************************************************************************
	 ; [BITS 16]
		ORG 0x7c00
	ENTRY:
	; code located at 0000:7C00, adjust segment registers and create stack
		cli
		xor		eax, eax
		mov		ss, ax
		mov		esp, ENTRY
		call	ena20
		lgdt	[gdt]
		mov		eax, cr0
		inc		ax
		mov		cr0, eax
		jmp		8:protm
	protm:
		mov		bx, 10h
		mov		ds, bx
		mov		es, bx
		mov		ss, bx
		dec		ax
		mov		cr0, eax
		jmp		unreal
	unreal:
		xor		ax, ax
		mov		es, ax
		mov		ds, ax
		mov		ss, ax
		sti

	; read image file into memory (1000:0000)
		mov		si, msgLoading
		call	DisplayMessage

		mov		bx, 2
		mov		cx, WORD [Sectors]
		xor		dx, dx

		mov     ax, 0x1000
		mov     es, ax					; destination for image
		call    ReadSectors
		mov		si, msgDone
		call	DisplayMessage

		push	WORD 0x1000
		push	WORD 0x0000
		retf
	FAILURE:
		mov		si, msgFailure
		call	DisplayMessage
		mov		ah, 0x00
		int		0x16					; await keypress
		int		0x19					; warm boot computer

	;*************************************************************************
	; PROCEDURE DisplayMessage
	; display ASCIIZ string at ds:si via BIOS
	;*************************************************************************
	DisplayMessage:
		lodsb							; load next character
		or      al, al					; test for NUL character
		jz      DONE
		mov     ah, 0x0E				; BIOS teletype
		mov     bx, 0x07				; text attribute 7, page 0
		int     0x10					; invoke BIOS
		jmp     DisplayMessage
	DONE:
		ret

	;*************************************************************************
	; PROCEDURE ReadSectors
	; reads <cx> sectors from disk starting at <bx> into memory location <es:0>
	;*************************************************************************
	ReadSectors:
		push	bx
		push	cx
		mov		al, 0x13
		sub		al, bl
		mov		cx, bx
		xor		bx, bx
		mov		ah, 2
		int		0x13
		jb		FAILURE
		mov		bx, es
		xor		ecx, ecx
		mov		cl, al
		shl		cx, 5
		add		bx, cx
		mov		es, bx
		pop		cx
		pop		bx
		xor		dh, 1
		jnz		other_head
		inc		bh

	other_head:
		mov	bl, 1
		xor	ah, ah
		sub	cx, ax
		jg	ReadSectors
		mov	al, 0x0C
		mov	dx, 0x3F2
		out	dx, al
		ret

	;*************************************************************************
	; PROCEDURE ena20
	; enables gate A20
	;*************************************************************************
	ena20:
		call	flush
		jnz		rretn
		mov		al, 0xD1
		out		0x64, al
		call	flush
		jnz		rretn
		mov		al, 0xDF
		out		0x60, al
	flush:
		mov		ecx, 0x20000
	tagain:
		jmp		delay
	delay:	
		in		al, 0x64
		test	al, 2
		loopnz	tagain
	rretn:	
		retn

	gdt			dw 0xFFFF
				dd gdt
				dw 0

				dw 0xFFFF
				dw 0
				dd 0x8F9A00

				dw 0xFFFF
				dw 0
				dd 0x8F9200

	msgLoading  db "Loading Boot Image ", 0x00
	msgDone     db "done", 0x0D, 0x0A, 0x00
	msgFailure  db 0x0D, 0x0A, "ERROR : Press Any Key to Reboot", 0x00

		TIMES 508-($-$$) DB 0x90
	Sectors		dw 30
	Signature	dw 0xAA55
	;*************************************************************************
