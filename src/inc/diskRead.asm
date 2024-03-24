; diskRead.asm
; Dependencies:
; print_string.asm

; Arguments:
; al = number of sectors to read
; dl = drive number
; es:bx = memory location to copy into
diskRead:
	push ax		; Push number of sectors REQUESTED for checking later
	mov ah, 0x2	; Disk read
	mov cl, 0x2	; Sector (0x1=boot, 0x2=first avail)
	mov ch, 0x0	; Cylinder
	; dl = drive number (may be set by qemu)
	; 0=floppy, 1=floppy2, 0x80=hdd, 0x81=hdd2
	mov dh, 0x0	; Head number

	int 0x13	; Call disk read
	jc .diskError

	pop bx
	cmp al, bl
	jne .sectorError 
	ret

.diskError:
	mov bx, MSGDISKERROR
	call printString
	jmp $
.sectorError:
	mov bx, MSGSECTORERROR
	call printString
	jmp $

MSGDISKERROR: db "Error reading disk.",0
MSGSECTORERROR: db "Error: wrong number of sectors.",0
