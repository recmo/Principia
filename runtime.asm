section .text
	extern malloc
	extern free

type_invalid:
	mov rax, 60  ; sys_exit
	mov rdi, 1   ; status
	syscall      ; Execution stops here

type_string:
	mov rax, 60  ; sys_exit
	mov rdi, 2   ; status
	syscall      ; Execution stops here

mem_alloc:
	; rsi contains the size (4 + rsi * 8)
	; rdi contains the return address
	; rsi contains the allocated address on return
	; All other registers need to be preserved
	; TODO
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
	mov ax, [rsi]      ; Load reference count
	or rax, rax        ; Test for zero
	jz .end            ; Skip if zero
	dec rax            ; Decrease reference count
	jz .free           ; Free if zero
	mov [rsi], ax      ; Store reference count
	pop rax            ; Restore rax
	.end:              ;
	jmp rdi            ; Return
	
	; Free closure
	.free:             ; Free the closure
	push rdi           ; Preserve rdi
	mov rdi, rsi       ; First argument passed in rdi
	; TODO: Recurse
	call free          ; Call free form stdlib
	; TODO: unclobber linux calling convetion clobbered registers
	pop rdi            ; Restore rdi
	jmp .end           ; Resume computation

mem_unpack:
	; rsi contains pointer to closure
	; rdi contains the return address
	; All other registers need to be preserved
	
	push rax           ; Preserve rax
	mov ax, [rsi]      ; Load reference count
	or rax, rax        ; Test for zero
	jz .end            ; Skip if zero
	dec rax            ; Decrease reference count
	jz .free           ; Free if zero
	mov [rsi], ax      ; Store reference count
	pop rax            ; Restore rax
	.end:              ;
	jmp rdi            ; Return
	
	; Free closure
	.free:             ; Free the closure
	push rdi           ; Preserve rdi
	mov rdi, rsi       ; First argument passed in rdi
	call free          ; Call free form stdlib
	; TODO: unclobber linux calling convetion clobbered registers
	pop rdi            ; Restore rdi
	jmp .end           ; Resume computation

exit:
	; Deref closure
	mov r12, rax
	mov r13, .next_1
	jmp mem_deref
	.next_1:
	
	; Syscall exit
	mov rax, 60        ; sys_exit
	mov rdi, 0         ; status
	syscall            ; Call kernel, never returns

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
	mov r12, rax
	mov r13, .next_1
	jmp mem_unpack
	.next_1:
	
	; Deref message
	mov r12, rbx
	mov r13, .next_2
	jmp mem_deref
	.next_2:
	
	; Call ret closure
	mov rax, rcx
	mov r15w, [rax + 2]
	jmp [function_table + r15 * 8]

read: ; ret
	
