org 0x20FF0

USE32

Gate1:
	; do something special here at Ring 3
  pop eax ; Get EFLAGS back into EAX. The only way to read EFLAGS is to pushf then pop.
  or eax, 0x200 ; Set the IF flag.
  push eax ; Push the new EFLAGS value back onto the stack. 
  mov eax, 0xdeadbeef

STOP:
  cli
  hlt
