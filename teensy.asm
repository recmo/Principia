; nasm -f elf64 teensy.asm
; ld -s teensy.o -o teensy
; ./teensy
;
; It is recommended to use the VDSO instead of syscalls directly, but
; looking at [1] it appears very few calls are optimized. This is confirmed
; with
;
;    objdump -T /lib/modules/(uname -r)/vdso/vdso64.so
;
; wich only lists gettimeofday, time, clock_gettime and getcpu.
;
; [1] https://en.wikibooks.org/wiki/X86_Assembly/Interfacing_with_Linux
; [2] /usr/include/x86_64-linux-gnu/asm/unistd_64.h
; [3] http://man7.org/linux/man-pages/man7/vdso.7.html

SECTION .data
	msg: db "Hello, World!",10
	len: equ $-msg

SECTION .text
	global _start

_start:
	mov rax, 1   ; sys_write
	mov rdi, 1   ; param 1 fd (unsigned int)
	mov rsi, msg ; param 2 buf (const char __user *)
	mov rdx, len ; param 3 count (size_t)
	;mov r10, 0  ; param 4 (unused)
	;mov r8,  0  ; param 5 (unused)
	;mov r9,  0  ; param 6 (unused)
	syscall      ; returns in rax, clobbers rcx and r11
	
	mov rax, 60  ; sys_exit
	mov rdi, 0   ; status
	syscall
