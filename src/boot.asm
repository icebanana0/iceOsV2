bits 16
[org 0x7c00]
KERNELOFFSET equ 0x1000	; Memory location we will load our kernel to

	mov bx, MSGREALMODE
	call printString

	; Load the kernel into memory from disk
	mov al, 1	; Load 1 sector (adjust for kernel size)
	; NOTE: Attempting to load too many sectors causes disk error in qemu
	mov bx, 0x0
	mov es, bx
	mov bx, KERNELOFFSET	; Target memory location of kernel
	call diskRead

	mov bx, MSGPROTECTEDMODE
	call printString

	call enterProtected

	jmp $

	%include "src/inc/printString.asm"
	%include "src/inc/diskRead.asm"
	%include "src/inc/gdt.asm"
	%include "src/inc/enterProtected.asm"

[bits 32]
pmStart:
	; Move execution into our actual kernel code by calling the memory location
	call KERNELOFFSET
	jmp $

MSGREALMODE db "OS started in 16-bit real mode.",0
MSGPROTECTEDMODE db "Entering protected mode...",0

	times 510-($-$$) db 0
	dw 0xaa55
