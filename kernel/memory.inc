bits 32

; memset: set each block of memory to a specific value per byte
; eax: pointer to a memory address
; ebx: value to set for each byte
; ecx: number of bytes to set
memset:
  sub ecx, 1
.loop:
  mov [eax + ecx], bl
  sub ecx, 1
  cmp ecx, -1
  jne .loop
.done:
  ret
