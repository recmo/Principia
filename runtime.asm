section .data

read_fixed: ; string
	dw 0x0000        ; reference_count (zero for static)
	dw 0x0006        ; function_index (type_string)
	dd 4             ; num_bytes
	db "TODO"

section .text
	extern realloc

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
	
	; Preserve all register clobbered by call except rsi
	push rax
	push rcx
	push rdx
	push rdi
	push r8
	push r9
	push r10
	push r11
	
	; Allocate
	mov rdi, 0     ; rdi = 0
	;mov rsi, rsi  ; rsi = rsi
	call realloc   ; rax = realloc(rdi, rsi)
	mov rsi, rax
	
	; Restore all registers clobbered by call except rsi
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rdx
	pop rcx
	pop rax
	
	; Initialize ref_count to one
	mov word [rsi], 1
	
	; Return
	.end:
	jmp rdi

mem_ref:
	; rsi contains pointer to closure
	; rdi contains the number of references to add (TODO)
	; rdi contains the return address
	; TODO: Test for zero and overflow
	; All other registers need to be preserved
	add word [rsi], di
	jmp rdi

mem_deref:
	; rsi contains pointer to closure
	; rdi contains the return address
	; All other registers need to be preserved
	
	push rax           ; Preserve rax
	xor rax, rax       ; Clear rax
	mov ax, word [rsi] ; Load reference count
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
	jmp .free
	push rdi           ; Preserve rdi
	mov rdi, rsi       ; First argument passed in rdi
	; TODO: Recurse
	;call free          ; Call free form stdlib
	; TODO: unclobber linux calling convetion clobbered registers
	pop rdi            ; Restore rdi
	jmp .end           ; Resume computation

mem_unpack:
	; rsi contains pointer to closure
	; rdi contains the return address
	; All other registers need to be preserved
	
	push rax           ; Preserve rax
	xor rax, rax       ; Clear rax
	mov ax, word [rsi] ; Load reference count
	or rax, rax        ; Test for zero
	jz .end            ; Skip if zero
	dec rax            ; Decrease reference count
	jz .free           ; Free if zero
	mov word [rsi], ax ; Store reference count
	.end:              ;
	pop rax            ; Restore rax
	jmp rdi            ; Return
	
	; Free closure
	.free:
	
	; Preserve all register clobbered by C function call
	; push rax ; Done separately
	push rcx
	push rdx
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	
	; Call realloc(rsi, 0)
	mov rdi, rsi       ; First argument passed in rdi
	mov rsi, 0         ; Second argument zero
	call realloc       ; rax = realloc(rdi, rsi)
	
	; Restore all registers clobbered by C function call
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	; pop rax ; Done separately
	
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
	
	; NOTE: Syscalls clobber rax, rcx, rdx, rsi, rdi, r8, r9, r10 and r11
	
	; Print message
	mov r12, rax       ; save rax
	mov rax, 1         ; sys_write
	mov rdi, 1         ; param 1 fd (unsigned int)
	mov rsi, r12       ; param 2 buf (const char __user *)
	add rsi, 8         ; offset 8
	mov edx, [r12 + 4] ; param 3 count (size_t)
	syscall            ; returns in rax, clobbers rcx and r11
	
	; Deref message
	mov rsi, r12
	mov rdi, .ret_1
	jmp mem_deref
	.ret_1:
	
	; Call ret closure
	mov rsi, rbx                   ; Closure from second argument
	xor rdi, rdi                   ; Clear rdi
	mov di, [rsi + 2]              ; Store function_index in rdi
	jmp [function_table + rdi * 8] ; Jump to function table entry

read: ; ret
	; Unpack closure
	mov rdi, .ret_0  ; return address
	jmp mem_unpack    ; closure in rsi
	.ret_0:
	
	; Fake by loading a fixed constant
	; defined above as read_fixed
	
	; Call [a0 C]
	mov rsi, rax
	mov rax, read_fixed
	mov di, [rsi + 2]
	jmp [function_table + rdi * 8]


