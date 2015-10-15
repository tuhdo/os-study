org 0x30FF0

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
  ; mov ecx, 0
  ; mov eax, 3
  ; mov edx, 0
  ; div dl

.test_intr_user_space:
  ; mov ecx, 1
  ; int 1

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
  mov ecx, .stop4
  sysenter

.stop4:
  mov eax, 4
  mov ecx, esp
  sysenter
