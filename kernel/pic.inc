%define IRQ_0	0x20				; IRQs 0-7 mapped to use interrupts 0x20-0x27
%define IRQ_8	0x28				; IRQs 8-15 mapped to use interrupts 0x28-0x36

; Initialization Control Word 1
%define ICW1_SEND_IC4 0x1
%define ICW1_SINGLE 0x2
%define ICW1_ADDRESS_INTERVAL_4 0x4 ; if set, use addresss inter, else 8
%define ICW1_LEVEL_TRIGGERED 0x8
%define ICW1_PIC_INITIALIZED 0x10
%define ICW1_IVT_ADDR1 0x20
%define ICW1_IVT_ADDR2 0x40
%define ICW1_IVT_ADDR3 0x80

; Initialization
; 1. write ICW1 to port 20h
; 2. write ICW2 to port 21h
; 3. if ICW1 bit D1=1  do nothing
; if ICW1 bit D1=0  write ICW3 to port 20h
; 4. write ICW4 to port 21h
; 5. OCW's can follow in any order
; http://stanislavs.org/helppc/8259.html
MapPIC:
  ; Setup ICW1
  mov al, (ICW1_SEND_IC4 | ICW1_PIC_INITIALIZED)
  out 0x20, al
  out 0xa0, al

  ; Setup ICW2
	; send ICW 2 to primary PIC
  ; the first 31 interrupts (0x0-0x1F) are reserved
	mov	al, IRQ_0		; Primary PIC handled IRQ 0..7. IRQ 0 is now mapped to interrupt number 0x20
	out	0x21, al

	; send ICW 2 to secondary controller
	mov	al, IRQ_8		; Secondary PIC handles IRQ's 8..15. IRQ 8 is now mapped to use interrupt 0x28
	out	0xa1, al

  ; Setup ICW3
  mov al, 0x4                   ; 0x4 = 0100 Second bit (IR Line 2)
  out 0x21, al  ; send to data register

  ; Send ICW 3 to secondary PIC
	mov	al, 0x2		; 0010=> IR line 2
	out	0xa1, al	; write to data register of secondary PIC

  ; Setup ICW4
	mov	al, 0x1		; bit 0 enables 80x86 mode

	; send ICW 4 to both primary and secondary PICs
	out	0x21, al
	out	0xA1, al

  ; All done. Null out the data registers
	; mov	al, 0
	; out	0x21, al
	; out	0xa1, al

  ; Disable all IRQs, except timer and keyboard
  mov al, 0xfc
  out 0x21, al
  out 0xA1, al

  ret

EnablePIC:
  mov eax, 0x0
  out 0x21, al
  ; out 0xa1, al
  ret
