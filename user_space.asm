org 0x20FF0

USE32
jmp Gate1

Gate1:
	; do something special here at Ring 3
  pop eax ; Get EFLAGS back into EAX. The only way to read EFLAGS is to pushf then pop.
  or eax, 0x200 ; Set the IF flag.
  push eax ; Push the new EFLAGS value back onto the stack. 
  mov ecx, esp
  mov edx, Stop
	sysenter

Stop:
  mov eax, 0
  mov ecx, esp
  mov edx, .stop1
  sysenter

.stop1:
  mov eax, 1
  mov ecx, esp
  mov edx, .stop2
  sysenter

.stop2:
  mov eax, 2
  mov ecx, esp
  sysenter
  ; cli
  ; hlt
