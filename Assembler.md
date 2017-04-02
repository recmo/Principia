
The registers of x86-64 are:

rax, rbx, rcx, rdx,
rbp, rsp, rsi, rdi,
r8,  r9,  r10, r11,
r12, r13, r14, r15

NOTE: `rsp` and `rbp` are conventionally used as the stack pointer and the
bottom of the stack.

NOTE: There are likely many more registers in silicon, these are used when
the processor is register-renaming.

# Calling convention

Only AMD64 is implemneted and supported.

## Linux System Calls

* The number of the syscall is passed in `rax`.
* Arguments are passed in `rdi`, `rsi`, `rdx`, `r10`, `r8` and `r9`, in that
order.
* A system call is done with the `syscall` instruction
* The parameter registers and `rcx` and `r11` are clobbered
* The register `rax` contains the result

## Usermode C/C++ function calls

* Depending on class of the argument (memory or integer) it is passed
  on the stack or in registers. Memory is passed on the stack, integer in
  registers.
* The interger arguments are stored in `rdi`, `rsi`, `rdx`, `rcx`, `r8`, and
  `r9` in order.
* `rsp` points to a stack with sufficient size
* Return value in `rax` and `rdx`
* The registers `rbp`, `rbx`, `r12`, `r13`, `r14`, `r15` are preserved
* So the registers `rax`, `rcx`, `rdx`, `rsp`, `rsi`, `rdi`, `r8`,  `r9`, `r10`
  and `r11` are clobbered.
* A call is done with `call`


## Principia

* The closure pointer is passed in `rsi`
* Arguments are passed in `rax`, `rbx`, `rcx`, `rdx`
* All regsters are clobbered
