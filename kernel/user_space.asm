org 0x20FF0

USE32
jmp Gate1

Gate1:
	; do something special here at Ring 3
  pop eax ; Get EFLAGS back into EAX. The only way to read EFLAGS is to pushf then pop.
  or eax, 0x200 ; Set the IF flag.
  push eax ; Push the new EFLAGS value back onto the stack. 
  mov ecx, esp
  mov edx, test_intr_user_space
	sysenter

test_intr_user_space:
  mov ecx, 1
  int 1

Stop:
  ; mov ax, 3
  ; mov dl, 0
  ; div dl
  mov ecx, 0
  mov eax, 3
  mov edx, 0
  div dl

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
  mov edx, .stop3
  sysenter

.stop3:
  mov eax, 3
  mov ecx, esp
  sysenter
