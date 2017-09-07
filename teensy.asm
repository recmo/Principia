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


; Multi-threading
; http://nullprogram.com/blog/2015/05/15/

; Closures in memory have the following layout:
;
;   reference_count  uint16
;   function_index   uint16
;   closure values   uint64*
;
; Special values:
;
;  * reference_count 0 means infinity (immutable, static, read-only)
;  * function_index  0 means invalid closure
;  * function_index  1 means the closure is acutually a string
;
; Closure with string literal format:
;
;   reference_count  uint16
;   function_index   uint16 (must be set to 1)
;   num_bytes        uint32
;

; Calling convention:
;
; The closure address is passed in rax
; arguments are passed in rbx, rcx, rdx

; rax, rbx, rcx, rdx, rbp, rsp, rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15

SECTION .data

function_table:
	dq type_invalid  ; 0
	dq type_string   ; 1
	dq mem_alloc     ; 2
	dq mem_ref       ; 3
	dq mem_deref     ; 4
	dq mem_unpack    ; 5
	dq exit          ; 6
	dq print         ; 7

exit_closure:
	dw 0x0000        ; reference_count (zero for static)
	dw 0x0006        ; function_index (exit)

print_closure:
	dw 0x0000        ; reference_count (zero for static)
	dw 0x0007        ; function_index (print)

hello_string:
	dw 0x0000        ; reference_count (zero for static)
	dw 0x0006        ; function_index (type_string)
	dd 0x0000000E    ; num_bytes
	db "Hello, World!",10

msg: db "Teensy.asm",10
len: equ $-msg

SECTION .text
	global main
	extern malloc
	extern free

main:
	mov rax, 1   ; sys_write
	mov rdi, 1   ; param 1 fd (unsigned int)
	mov rsi, msg ; param 2 buf (const char __user *)
	mov rdx, len ; param 3 count (size_t)
	;mov r10, 0  ; param 4 (unused)
	;mov r8,  0  ; param 5 (unused)
	;mov r9,  0  ; param 6 (unused)
	syscall      ; returns in rax, clobbers rcx and r11
	
	; Load print with closure, 
	mov rax, print_closure ; load print_closure
	mov rbx, hello_string  ; First argument: hello string closure
	mov rcx, exit_closure  ; Second argument: exit closure
	
	; Call
	mov r15w, [rax + 2]
	jmp [function_table + r15 * 8]

type_invalid:
	mov rax, 60  ; sys_exit
	mov rdi, 1   ; status
	syscall
	; Execution stops here

type_string:
	mov rax, 60  ; sys_exit
	mov rdi, 2   ; status
	syscall
	; Execution stops here

mem_alloc:
	nop

mem_ref:
	nop

mem_deref:
	jmp r11

mem_unpack:
	nop

exit:
	; Deref closure
	mov r11w, [rax]    ; Load reference count
	or r11, r11        ; Test for zero
	jz .end_1          ; Skip if zero
	dec r11            ; Decrease reference count
	jz .free_1         ; Free if zero
	mov [rax], r11w    ; Store reference count
	.end_1:            ; Resume without closure
	
	; Syscall exit
	mov rax, 60        ; sys_exit
	mov rdi, 0         ; status
	syscall            ; Call kernel
	; Execution stops here
	
	; Free closure
	.free_1:           ; Free the closure
	mov rdi, rax       ; First argument passed in rdi
	call free          ; Call free form stdlib
	jmp .end_1         ; Resume computation

print: ; message, ret
	
	; Print message
	mov r12, rax       ; save rax
	mov r13, rcx       ; save rcx
	mov rax, 1         ; sys_write
	mov rdi, 1         ; param 1 fd (unsigned int)
	mov rsi, rbx       ; param 2 buf (const char __user *)
	add rsi, 8         ; offset 8
	mov edx, [rbx + 4] ; param 3 count (size_t)
	syscall            ; returns in rax, clobbers rcx and r11
	mov rax, r12       ; restore rax
	mov rcx, r13       ; restore rcx
	
	; Deref closure
	mov r11w, [rax]    ; Load reference count
	or r11, r11        ; Test for zero
	jz .end_1          ; Skip if zero
	dec r11            ; Decrease reference count
	jz .free_1         ; Free if zero
	mov [rax], r11w    ; Store reference count
	.end_1:            ; Resume without closure
	
	; Deref message
	mov r11w, [rbx]
	or r11, r11
	jz .end_2
	dec r11
	jz .free_2
	mov [rbx], r11w
	.end_2:
	
	; Call ret closure
	mov rax, rcx
	mov r15w, [rax + 2]
	jmp [function_table + r15 * 8]
	
	.free_1:
	mov rdi, rax
	call free
	jmp .end_1
	
	.free_2:
	mov rdi, rbx
	call free
	jmp .end_2
