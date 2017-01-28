; make; and ./Principia machine examples/FizzBuzz.principia  > FizzBuzz.asm; and nasm -f elf64 FizzBuzz.asm; and gcc FizzBuzz.o -o FizzBuzz; valgrind ./FizzBuzz

section .rodata

read_fixed: ; string
	dw 0x0000        ; reference_count (zero for static)
	dw 0x0006        ; function_index (type_string)
	dd 4             ; num_bytes
	db "TODO"

exit_closure:
	dw 0x0000        ; reference_count (zero for static)
	dw 0x0005        ; function_index (exit)

section .data

current_brk:
	dq 0

free_list:
	dq 0

section .text
	global _start

_start:
	; Compute the current break
	mov rax, 12       ; sys_brk
	mov rdi, 0        ; void* addr
	syscall           ; returns in rax, clobbers rcx and r11
	mov [current_brk], rax ; Store in current_brk
	
	; Call main closure with exit as argument
	mov rsi, main_closure
	mov rax, exit_closure
	movzx rdi, word [rsi + 2]      ; Store function_index in rdi
	jmp [function_table + rdi * 8] ; Jump to function table entry

type_invalid:
	mov rax, 60  ; sys_exit
	mov rdi, 1   ; status
	syscall      ; Execution stops here

type_string:
	mov rax, 60  ; sys_exit
	mov rdi, 2   ; status
	syscall      ; Execution stops here

mem_alloc:
	; rsi contains the size rsi = 4 + n * 8
	; rdi contains the return address
	; rsi contains the allocated address on return
	; All other registers need to be preserved
	
	; Check the free_list
	mov rsi, [free_list]       ; Load current free object
	or rsi, rsi                ; Test for zero
	jz .new                    ; Create new if zero
	
	; Pop from the free list
	push rax
	mov rax, [rsi]             ; Load pointer to next free object
	mov [free_list], rax       ; Store in free_list
	pop rax
	jmp .end
	
	.new: ; Allocate new space
	
	; Compute the new break in rdi
	push rax
	push rcx
	push rdx
	;push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
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
	mov rsi, rax
	
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rdx
	pop rcx
	pop rax
	
	; Return
	.end:
	mov word [rsi], 1   ; Initialize ref_count to one
	jmp rdi

mem_deref:
	; rsi contains pointer to closure
	; rdi contains the return address
	; All other registers need to be preserved
	
	push rax           ; Preserve rax
	movzx rax, word [rsi] ; Load reference count
	or rax, rax        ; Test for zero
	jz .end            ; Skip if zero
	dec rax            ; Decrease reference count
	jz .free           ; Free if zero
	mov word [rsi], ax ; Store reference count
	.end:              ;
	pop rax            ; Restore rax
	jmp rdi            ; Return
	
	; Free closure
	.free:             ; Free the closure
	
	; Recursively call mem_deref on all values
	
	push rbx
	push rcx
	push rsi
	push rdi
	
	movzx rax, word [rsi + 2] ; Load funcion_index
	movzx rbx, byte [size_table + rax] ; Load the closure size
	or rbx, rbx
	jz .end_loop           ; Skip if no values
	mov rcx, rsi
	add rcx, 4             ; Point rcx to the first value
	
	.loop:                 ; For each value in the closure
	mov rsi, [rcx]         ; Load value (pointer to other closure)
	mov rdi, .ret          ; Return to loop
	jmp mem_deref          ; Recurse on closure
	.ret:                  ;
	add rcx, 8             ; Increase value pointer
	dec rbx                ; Decrease loop counter
	jnz .loop              ; Repeat if non-zero
	.end_loop:
	
	pop rdi
	pop rsi
	pop rcx
	pop rbx
	
	; Push [rsi] onto free list
	mov rax, [free_list]
	mov [rsi], rax
	mov [free_list], rsi
	
	jmp .end           ; Resume computation

mem_unpack:
	; rsi contains pointer to closure
	; rdi contains the return address
	; All other registers need to be preserved
	
	push rax           ; Preserve rax
	movzx rax, word [rsi] ; Load reference count
	or rax, rax        ; Test for zero
	jz .end            ; Skip if zero
	dec rax            ; Decrease reference count
	jz .free           ; Free if zero
	mov word [rsi], ax ; Store reference count
	
	; Issue additional references to the values if we keep the closure
	; arround after unpacking
	push rbx
	push rcx
	push rdx
	movzx rax, word [rsi + 2] ; Load funcion_index
	movzx rcx, byte [size_table + rax] ; Load the closure size
	or rcx, rcx
	jz .end_loop           ; Skip if no values
	mov rbx, rsi
	add rbx, 4             ; Point rcx to the first value
	
	.loop:                 ; For each value in the closure
	mov rdx, [rbx]         ; Load value (pointer to other closure)
	movzx rax, word [rdx]  ; Load ref_count
	or rax, rax            ; Test for zero
	jz .skip               ; Skip if zero
	add rax, 1             ; Increase reference count
	mov word [rdx], ax     ; Store reference count
	.skip:                 ;
	add rbx, 8             ; Increase value pointer
	loop .loop             ; Decrement rcx and jump to .loop if nonzero
	
	.end_loop:
	pop rdx
	pop rcx
	pop rbx
	
	.end:              ;
	pop rax            ; Restore rax
	jmp rdi            ; Return
	
	; Free closure
	.free:
	
	; Push [rsi] onto free list
	mov rax, [free_list]
	mov [rsi], rax
	mov [free_list], rsi
	
	jmp .end           ; Resume computation

exit:
	; Unpack closure
	mov rdi, .ret_0  ; return address
	jmp mem_unpack    ; closure in rsi
	.ret_0:
	
	; Syscall exit
	mov rax, 60        ; sys_exit
	mov rdi, 0         ; status
	syscall            ; Call kernel, never returns

print: ; message, ret
	; Unpack closure
	mov rdi, .ret_0    ; return address
	jmp mem_unpack     ; closure in rsi
	.ret_0:
	
	; Print message
	mov r12, rax       ; save rax
	mov rax, 1         ; sys_write
	mov rdi, 1         ; param 1 fd (unsigned int)
	mov rsi, r12       ; param 2 buf (const char __user *)
	add rsi, 8         ; offset 8
	mov edx, [r12 + 4] ; param 3 count (size_t)
	syscall            ; returns in rax, clobbers rcx and r11
	
	; NOTE: Syscalls clobber rax, rcx, rdx, rsi, rdi, r8, r9, r10 and r11
	
	; Deref message
	mov rsi, r12
	mov rdi, .ret_1
	jmp mem_deref
	.ret_1:
	
	; Call [a1]
	mov rsi, rbx                   ; Closure from second argument
	movzx rdi, word [rsi + 2]      ; Store function_index in rdi
	jmp [function_table + rdi * 8] ; Jump to function table entry

read: ; ret
	; Unpack closure
	mov rdi, .ret_0  ; return address
	jmp mem_unpack    ; closure in rsi
	.ret_0:
	
	; Fake by loading a fixed constant
	; defined above as read_fixed
	
	; Call [a0 C]
	mov rsi, rax                   ; Closure from second argument
	mov rax, read_fixed            ; Pass read_fixed closure as the first argument
	movzx rdi, word [rsi + 2]      ; Store function_index in rdi
	jmp [function_table + rdi * 8] ; Jump to function table entry

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
