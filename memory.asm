; Simple memory allocator interfacing directly with the kernel
; Allocates closures of a fixed size (36 bytes, enough for a 4-closure)

%define size 36

section .data

current_brk:
	dq 0

free_list:
	dq 0

section .text
	global malloc
	global free

init:
	; Compute the current break
	mov rax, 12       ; sys_brk
	mov rdi, 0        ; void* addr
	syscall           ; returns in rax, clobbers rcx and r11
	mov [current_brk], rax ; Store in current_brk
	ret

malloc:
	; Check the free_list
	mov rax, [free_list]       ; Load current free object
	or rax, rax                ; Test for zero
	jz .new                    ; Create new if zero
	
	; Pop from the free list
	mov rdi, [rax]             ; Load pointer to next free object
	mov [free_list], rdi       ; Store in free_list
	ret                        ; Return current free object (in rax)
	
	.new: ; Allocate new space
	
	; Compute the new break in rdi
	mov rdi, [current_brk]
	add rdi, 36
	
	; Call brk
	mov rax, 12       ; sys_brk
	;mov rdi, 0       ; void* addr
	syscall           ; returns in rax, clobbers rcx and r11
	; NOTE: Syscalls clobber rax, rcx, rdx, rsi, rdi, r8, r9, r10 and r11
	
	; On success, rax contains the new break, else it contains the current break
	;cmp rax, rdi
	; TODO test
	
	mov [current_brk], rax
	sub rax, 36
	ret

free:
	; rdi: pointer to closure
	
	; Push onto free list
	mov rax, [free_list]
	mov [rdi], rax
	mov [free_list], rdi
	
	; TODO: Free list
	
	ret

malloc_fail:
	mov rax, 60  ; sys_exit
	mov rdi, 3   ; status
	syscall      ; Execution stops here
