# Principia

	hello_world ↦ “Hello, World!”

Futuristic programming, reasoning and proof language.


### About the name

The name is a reference to Whitehead & Russel's [Principia Mathemica](https://en.wikipedia.org/wiki/Principia_Mathematica). ‘Mathematica’ was already taken by another cool programming language, so I choose ‘Principia’. It fits, because this language is focused more on principles and less on mathematics. (Although I envision a fully-proven high performance computer algebra library to be one of the first applications).


## Syntax

The syntax is minimalist and heavily uses the [Unicode Standard](http://www.unicode.org/versions/Unicode6.2.0/). Please use something like [compose keys](https://en.wikipedia.org/wiki/Compose_key) to enter Unicode characters (here's my setup: [XCompile](https://github.com/Recmo/XCompile)).

The language is a union between a programming language and a proof language. The programming language consists of just two constructs, *calls* and *closures*. It has eager evaluation, first-class functions and multiple return values. The proof language has four constructs, *because*, *axiom*, *therefore* and *proofs*. Both these languages live in a space that is similar to a directed cyclic multi-graph.

Unifying these languages is a meta-language that turns these graphs into a linear syntax for consumption by text editors. The meta-language handles scoping and identifier binding. It should be noted that binding is separate from the programming language. The programming language has *no* concept of variables and binding (and therefore, none of the problems associated with it).



## Programming language

### Call statement

	return₁ return₂ … ≔ function argument₁ argument₂ …

The call statement calls the function in inbound position one with the remaining inbounds as arguments. The return values are assigned to the outbounds.

If the arity of the closure mismatches the call, the arguments and retun values are padded with `(↦)`.

### Closure statement

	function argument₁ argument₂ … ↦ return₁ return₂ …

The closure statement is perfectly symmetrical to the call statement. Now, the function and it's arguments are outbound and the return values are inbound.

### On eagerness

The language is eager


## Object oriented programming and other conventions

* [Scott encoding](https://en.wikipedia.org/wiki/Mogensen%E2%80%93Scott_encoding)



## Proof language

**To be written**

### Because (pre-conditions)

	∵ proposition

### Therefore (post-conditions)

	∴ proposition

### Axiom

	⊨ proposition

### Proofs (assertions)

	⊢ proposition


## Implementation

* C++11 (Clang)
* Queχ (only if you regenerate the parser)
* LLVM (MCJIT)

The Queχ based parser produces an abstract syntax tree (AST). The binder then binds all the identifiers in the AST. From this the program data flow graph (DFG) is created. The DFG is the natural representation of the program and is where static verification and optimizations like constant propagation happen.

From this DFG the (nested) closures are extracted and lambda lifted. The lifted closures are then serialized and converted to a simple stack machine language. The stack machine language is compiled to LLVM intermediate representation (IR). The IR run through optimization passes and JIT compiled to native machine code.

## How to build and run

	git clone git@github.com:Recmo/Principia.git
	cd Principia
	make -j4
	./Principia Ackermann.txt PRA 3 3

## Appendix: Recommended compose bindings

For programming in this Unicode heavy language I use XCompose. My compose key is `caps lock` which is otherwise disabled.

 Symbol  Key sequence
-------- --------------------------
  `↦`    `caps lock` `|` `→`
  `≔`    `caps lock` `:` `=`
  `·`    `caps lock` `c` `.`

## Appendix: Unicode symmetric pairs

https://en.wikipedia.org/wiki/Bracket

( ) [ ] { } < > ‘ ’ “ ” ⸤ ⸥ ｢ ｣ ⟦ ⟧ ⟨ ⟩ 【 】 ⟦ ⟧ ⟨ ⟩ ⟪ ⟫ ⟬ ⟭ ⟮ ⟯ ⦃ ⦄ ⦅ ⦆ ⦇ ⦈ ⦉ ⦊ ⦋ ⦌ ⦍ ⦎ ⦏ ⦐ ⦑ ⦒ ⦓ ⦔ ⦕ ⦖ ⦗ ⦘  ⌈ ⌉ ⌊ ⌋ ⌌ ⌍ ⌎ ⌏ ⌜ ⌝ ⌞ ⌟
