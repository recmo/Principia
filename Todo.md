# To do lists

## Virtual machine
[x] Compile to stack based language
[x] Execute in stack based language
[x] Unify imports and constants
[x] Virtual stack
[x] Remove the stack
[x] Seperate numbering for constants, closures, arguments and allocs
[x] Promote constant allocs to constant closures
[x] Add address_t type and use it
[x] Inline functions
[x] Fixed-point constant finding and inlining
[x] Global list of constants
[x] Deduplicated constants
[x] Remove unused allocs
[x] Explicit reference count instructions
[x] Explicit reference counting (instead of std::shared_ptr)
[x] Allocate closures as header + values
[x] Remove unused functions
[x] Remove unused closure values
[x] Inline calls to allocs
[x] Remove unused closure places
[x] Deduplicate allocs
[x] Compile to assembler (see teensy.asm)
[x] Implement assembler mem_alloc
[x] Implement assemble mem_unpack
[x] Generate refs
[x] Fix register assignment order for arguments to arguments
[x] Recursive assembler mem_unpack
[x] Recursive assembler mem_deref
[x] Simple free-list allocator, fixed size, using sys_brk
[x] Clear rsi on first ref only
[x] Direct jmp when target func is known (const, alloc)
[x] Allocate memory in chunks of one megabyte (to avoid many sys_brk)
[x] Remove function_table indirection
[x] Abolish all conventional use of stack
[x] Pass closures in rsp on call
[x] Use scratch memory for reg save/restore instead of stack
[x] mem_alloc returns new closure in rsp
[ ] Statically allocate closures that exist at most once
[ ] Specialize functions of constant closures by inlining to get constant empty closures
[ ] Unrecurse mem_deref
[ ] Add ref_count to alloc_instruction_t
[ ] Shuffle call arguments by finding cycle decompositions and using xchg
[ ] Refcount before closure (ie offset pointers by two)
[ ] Generate mem_*_n functions and alloc size based on maximum closure size
[ ] Have unrolled mem_unpack_n and mem_deref_n functions
[ ] Calling convention passing closure arguments in registers
[ ] Skip setting closure pointer when calling funcs with constant empty closures
[ ] Use rsp to point to closures, use ret to jump to them, pop to unpack them
[ ] Control flow analysis, to find more constants
[ ] Deduplicate functions (Is there a unique reduced form?)
[ ] Inline memory
[ ] Handle overflow in assembler ref
[ ] Implement assembler read
[ ] Allocate memory using syscalls
[ ] Implement fixed-size pool allocators with free-lists
[ ] Abuse rsp, push and pop for memory management
[ ] Benchmarking per closure, histogram per function
[ ] Promote closure alllocs to closure values (closure-inlining)
[ ] Reference counting memory manager
[ ] Statically analyse possible functions at tail call site
[ ] Statically analyse closure life-times
[ ] Investigate different memory allocation strategies

## Language
[ ] Add read intrinsic, similar to print for write
[ ] Add intrinsics for string processing
[ ] Add file handles for read/write
[ ] Add Linux syscall mechanism
[ ] Add proof language
[ ] Code over multiple files
[ ] Mechanism for code modules

# Calling convention

Arguments are passed in registers, if there are not enough registers, the
remaining arguments are passed through static global variables. Since we know
in advance what the the maximum number of arguments is going to be, we can
allocate this array staticall (if required at all).

If the function has a non-empty closure, a pointer to the closure is passed as
the first argument.

Idea: pass the closure values in registers as well.

Idea: Re-order arguments such that at runtime, the minimum amount of register
shuffling is required. Use profiling data to determine how to order.

Idea: Re-order functions such that fall-through instead of a jump to share code
between an inlined function and the un-inlined version (at least for one
location where it is inlined). Use profiling data to determine which function.

Idea: Provide a native "syscall" function, use this to implement an stdlib.
http://syscalls.kernelgrok.com/
See teensy.asm

This is similar to how Jonesforth operates: http://git.annexia.org/?p=jonesforth.git;a=blob;f=jonesforth.S

/// TODO: Use alloca when we can

/// TODO: Background thread to analyse the performance oprofile style
/// TODO: Background thread to dynamically optimize code

/// TODO: Find a contrived example where the validity of the mutual recursion depends on an unsolved problem.
///       f x ↦ (≔(≔ if (↦(≔ complicated_function_1 x)) (↦(≔ g x)) (↦x))
///       g x ↦ (≔(≔ if (↦(≔ complicated_function_2 x)) (↦(≔ f x)) (↦x))
/// Then add a variable to force the closure to be dynamic.
/// TODO: Find a contrived example where a partially filled closure might still be executed.
/// TODO: Proof that finding an executable closure storing order is equivalent to the halting problem.

/// TODO: McCarthy 91 function

// M n ↦ (≔(≔ if (≔ greater n 100) (↦(≔ sub n 10)) (↦(≔ M (≔ M (≔ add n 11))))))


/// TODO: Add proof language

/*

Hoare Logic:

f a ↦ b
	Theorems A(a) about a
	Derivations / proofs
	Theorems B(b) about b

Theorems Y(y) about y
x ≔ f y
	Verify A(y) ⊆ Y(y)
	Therefore B(x)

How do we do derivations?

IDEA: We can use abstract functions, i.e. functions without implementations
 to represent (non-constructive) theorems.
This function should generally return just the input variables,
however, it may return new values. This would be an existence proof!

How do we do axioms?

IDEA: We omit the derivations from the function.

This gives four kinds of functions

- Without implementation and without derivation. (no-op)
- With implementation and without derivation. (unproven construction)
- Without implementation and with derivation. (abstract/pure/nonconstructive proof)
- With implementation and with derivation. (plain function with proof)

*/


// Boolean edges as truth values
// Theorems as 

// Implement boolean values as:
// True a b ↦ a
// False a b ↦ b
// Then the following are equivalent:
// a ≔ if condition then else
// a ≔ condition then else

// IDEA: False = the non-halting function, true = everything else


/*

TODO: Namespaces and other sugar.

Namespaces = named scopes!

f a b ↦ r
	x …
	y …

then allows the global scope to access the constants:

f.x
f.y


Simmilarly, one can define a namespace:

f ↦
	x …
	y …

TODO: importing namespaces:

import int Principia.Integers

int.add a b


TODO: process docstrings:

ignore “uncommented piece of code”

doc symbol “Documentation”


TODO: Reflection

a ≔ parse “some piece of source code”

doc parse “Returns a closure representing the parsed source code, all unbound variables are arguments, all exported symbols are returns.”

c ≔ link a b

doc link “Go trough all arguments of the closure a and link them to variables in scope b. Unlinked symbols remain.”

rets ≔ eval f args

doc eval “Execute f with arguments args and return the results in rets.”


TODO: Multiple implementations (algorithms) of the same function

May not differ in the function specification. Must contain full proofs. May differ in runtime resource usage.

The compiler is free to change between different implementations of a function as it sees fit (optimizing). The programmer can provide hints to direct this choice.

Alternative: Functions have a unique implementation, but one can proof equivalences, which the compiler will then use.

Suppose we have two functions, safediv and unsafediv, where the later is faster, but has the additional precondition that m ≠ 0. If the compiler can deduce statically or runtime that always m ≠ 0, then it may move to the unsafediv.


TODO: Types‽ What do we mean we we say “pre n : integer”?
That there exists functions +, -, ×, |·|, =, ≠, <, ≤, >, ≥, etc… doing the ‘expected’ thing.

They can be implemented as unspecified propositions: IsInteger(n). Dependent types can be implemented as more complex variants: IsMatrix(A, ℂ, 2, 4) to state that A is a complex valued 2 × 4 matrix.


TODO: Performance characteristics:

slowdiv n m ↦ q r
	complexity.time slowdiv log(n) + log(m) 
	complexity.memory slowdiv 2 * m


TODO: Compiler hints

intdiv n m ↦ q r
	hint intdiv hint.Inline
	hint intdiv hint.Tailrecurse
	force intdiv force.Inline


TODO: Something like the Common Lisp Object System and "The Art of the Metaobject Protocol"


TODO: Allow syntax modifications in language:

Lexer -> Preprocessor -> Parser
Lexer: Chunk source into identifiers
Preprocessor: resolve scoping and references
Parser: 


[parse rule:  #1 + #2 ↦ (. ≔ plus #1 #2) ]
[parse rule:  if #1 then #2 else #3  ↦ (≔(≔if #1 (↦ #2) (↦#3))) ]
[parse rule:  /#1/ ↦ ( . ≔  regexp_parse("#3") ) ]
etc…

