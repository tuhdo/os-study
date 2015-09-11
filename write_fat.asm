mov ah, 0bh
mov bh, 00h
mov bl, 0ffh
int 0x10
cli							; Clear all Interrupts
hlt							; halt the system
