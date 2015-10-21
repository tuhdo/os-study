	;*************************************************
	;	Gdt.inc
	;		-GDT Routines
	;
	;	OS Development Series
	;*************************************************

%ifndef __GDT_INC_67343546FDCC56AAB872_INCLUDED__
%define __GDT_INC_67343546FDCC56AAB872_INCLUDED__

bits	16

	;*******************************************
	; InstallGDT()
	;	- Install our GDT
	;*******************************************

InstallGDT:

	cli				; clear interrupts
	pusha				; save registers
	lgdt	[toc]			; load GDT into GDTR
	sti				; enable interrupts
	popa				; restore registers
	ret				; All done!

	;*******************************************
	; Global Descriptor Table (GDT)
	;*******************************************

gdt_data:
	dd 0				; null descriptor
	dd 0

	; gdt code:				; code descriptor
	dw 0FFFFh				; limit low
	dw 0				; base low
	db 0x01					; base middle
	db 10011010b			; access
	db 11001111b			; granularity
	db 0				; base high

	; gdt data:				; data descriptor
	dw 0FFFFh				; limit low (Same as code)
	dw 0				; base low
	db 0x0				; base middle
	db 10010010b			; access
	db 11001111b			; granularity
	db 0				; base highi

	; User Space code (Offset: 24 (0x18) bytes)
	dw 0FFFFh				; limit low
	dw 0				; base low
	db 0x4				; base middle
	db 11111110b			; access - Notice that bits 5 and 6 (privilege level) are 11b for Ring 3
	db 11001111b			; granularity
	db 0				; base high

	; User Space data (Offset: 32 (0x20) bytes
	dw 0FFFFh				; limit low (Same as code)
	dw 0				; base low
	db 0x00				; base middle
	db 11110010b			; access - Notice that bits 5 and 6 (privilege level) are 11b for Ring 3
	db 11001111b			; granularity
	db 0				; base high

	; video memory: 40  (0x28)
	dw 0x7FFF  										; limit
	dw 0x8000												; base low
	db 0x0B												; base middle
	db 11110010b
	db 11001111b									; granularity
	db 0													; base high

	; INT0: 48 (0x30)
	dw 0FFFFh				; limit low
	dw 0				; base low
	db 0x02					; base middle
	db 10011010b			; access
	db 11001111b			; granularity
	db 0				; base high

	; call gate to kernel space: 56 (0x38)
	dw 0FFFFh				; limit low
	dw 0				; base low
	db 0x01					; base middle
	db 11111010b			; access
	db 11001111b			; granularity
	db 0				; base high

	; End of the GDT. Define the routine wherever
	; The call gate routine

end_of_gdt:

toc:
	; according to lgdt description, the instruction accepts a 6-bytes
	; operand, with the lowest 2 bytes size of GDT, and the remaining
	; 4 bytes the base address of GDT
	dw end_of_gdt - gdt_data - 1	; limit (Size of GDT)
	dd gdt_data				; base of GDT


%endif ;__GDT_INC_67343546FDCC56AAB872_INCLUDED__
