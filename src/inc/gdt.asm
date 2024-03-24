; GDT - Global Descriptor Table
gdtStart:
gdtNull:	; Entry 1: Null entry must be included first (error check)
	dd 0x0	; double word = 4 bytes = 32 bits
	dd 0x0
gdtCode:	; Entry 2: Code segment descriptor
	; Structure:
	; Segment Base Address (base) = 0x0
	; Segment Limit (limit) = 0xfffff
	dw 0xffff	; Limit bits 0-15
	dw 0x0000	; Base bits 0-15
	db 0x00		; Base bits 16-23
	; Flag Set 1:
		; Segment Present: 0b1
		; Descriptor Privilege level: 0x00 (ring 0)
		; Descriptor Type: 0b1 (code/data)
	; Flag Set 2: Type Field
		; Code: 0b1 (this is a code segment)
		; Conforming: 0b0 (Code w/ lower privilege may not call this)
		; Readable: 0b1 (Readable or execute only? Readable means we can read code constants)
		; Accessed: 0b0 (Used for debugging and virtual memory. CPU sets bit when accessing segment)
	db 10011010b	; Flag set 1 and 2
	; Flag Set 3
		; Granularity: 0b1 (Set to 1 multiplies limit by 4K. Shift 0xfffff 3 bytes left, allowing to span full 32G of memory)
		; 32-bit default: 0b1
		; 64-bit segment: 0b0
		; AVL: 0b0
	db 11001111b	; Flag set 3 and limit bits 16-19
	db 0x00		; Base bits 24-31
gdtData:
	; Same except for code flag:
		; Code: 0b0
	dw 0xfffff	; Limit bits 0-15
	dw 0x0000	; Base bits 0-15
	db 0x00		; Base bits 16-23
	db 10010010b	; Flag set 1 and 2
	db 11001111b	; 2nd flags and limit bits 16-19
	db 0x00		; Base bits 24-31

gdtEnd:		; Needed to calculate GDT size for inclusion in GDT descriptor

; GDT Descriptor
gdtDescriptor:
	dw gdtEnd - gdtStart - 1	; Size of GDT, always less one
	dd gdtStart

; Define constants
CODESEG equ gdtCode - gdtStart
DATASEG equ gdtData - gdtStart

; In protected mode, set DS = INDEX to select GDT entries
; Then CPU knows to use segment at that offset
; Example: (0x0: NULL segment; 0x8: CODE segment; 0x10: DATA segment)
