; nasm -f elf64 teensy.asm
; gcc -Wall -s -nostdlib -nostartfiles -nostdlib -nodefaultlibs teensy.o -o teensy
; strip -s ./teensy
; ./teensy
; TODO: Port to x86-64
; See https://stackoverflow.com/questions/2535989/what-are-the-calling-conventions-for-unix-linux-system-calls-on-x86-64

SECTION .data
	msg:	db "Hi World",10
	len:	equ $-msg

SECTION .text
	global _start
_start:
	mov rdx,len
	mov rcx,msg
	mov rbx,1
	mov rax,4
	int 0x80
	
	mov rbx,0
	mov rax,1
	int 0x80
