bits	16

InstallIDT:
  cli				; clear interrupts
	pusha				; save registers
  lidt	[idt_ptr]			; load GDT into GDTR
  sti
  popa
  ret

idt_start:
  ;; IRQ0
  dw   0
  dw   0x30
  db   0
  db   011001110b
  dw   0

  ;; IRQ1
  dw   0
  dw   0x30
  db   0
  db   011001110b
  dw   0

;; IRQ2
  dw   0
  dw   0x30
  db   0
  db   011001110b
  dw   0

idt_end:

idt_ptr:
	.limit	dw idt_end - idt_start	; bits 0...15 is size of idt
	.base	dd idt_start		; base of idt

  ; load register with idt_ptr
