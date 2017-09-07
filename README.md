# Principia

	hello_world ↦ “Hello, World!”

Futuristic programming, reasoning and proof language.

## Inspiration

* Metamath (minimalist proof language)
* Python (indentation based syntax)
* Haskell
* Agda
* Idris
* Coq
* Forth
* Lisp (minimalist language)


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


Nothing return ↦ return (nothing just ↦ nothing)

Just value return ↦ return (nothing just ↦ just value)


λ main exit
	print “Hello” (λ)
	input ”Name?” (λ name)
	print “Hello, ” (λ)
	print name (λ)
	exit


main exit ↦ print “Hello” (↦
	input ”Name?” (name ↦
		print “Hello, ” (↦
			print name (↦
				exit
			)
		)
	)
)


main exit ↦
	print “Hello” (↦
		input ”Name?” (name ↦
			print “Hello, ” (↦
				print name (↦
					exit
				)
			)
		)
	)


select cond then else return


main exit ↦
	print “Hello”;
	input ”Name?”; name
	input ”Is it morning?”; morning
	if morning (↦ cnt “Good morning, ”) (↦ cnt “good day”)
	cnt day ↦
		print day;
		print name;
		exit


⊥ ↦ ⊥

nop return ↦ return

id₀ return ↦ return
id₁ a return ↦ return a
id₂ a b return ↦ return a b
id₃ a b c return ↦ return a b c

pack₀ return ↦ return (return ↦ return)
pack₁ a return ↦ return (return ↦ return a)
pack₂ a b return ↦ return (return ↦ return a b)
pack₃ a b c return ↦ return (return ↦ return a b c)

ternary condition a b return ↦ if condition (↦ return a) (↦ return b)


doc string return ↦ return
arg argument description return ↦ return

doc “Annotations are variants of nop with extra arguments. Annotations can
take any number of arguments. There is no special compiler support for them,
a simple inlining pass will remove them. But you can write compiler extension
operating on the program graph that can give them meaning. Anything that
resolves in the local scope can be passed to them (since they are no different
from any other call). This is how the proof verifier will be implemented.”

fibonacci n return ↦ .
	doc “Implements the Fibonacci function.” (↦ .)
	arg n “The Fibonacci number to compute.” (↦ .)
	typeof n Int (↦ .)

aᵢ ,\n bᵢ ≡ ‘aᵢ bᵢ’
aᵢ : bᵢ ≡ ‘bᵢ ↦ ,’
aᵢ ; bᵢ ≡ ‘aᵢ (bᵢ ↦ ,)’
aᵢ ⇒ bᵢ ≡ `aᵢ return ↦ return bᵢ`


fibonacci n return:
	doc “Implements the Fibonacci function.”;
	arg n “The Fibonacci number to compute.”;
	typeof n Int;
	mul a b; m

, ≡ continues on next line
aᵢ ≔ bᵢ ≡ ‘bᵢ (aᵢ ↦ ,)’

fibonacci n return ↦,
	≔ doc “Implements the Fibonacci function.”
	≔ arg n “The Fibonacci number to compute.”
	≔ typeof n Int
	m ≔ mul a b

Con: Causal order disappears. m doesn't exist before mul, yet stands before mul.
Pro: Easy to find definition point of variables.

, ≡ continues on next line
aᵢ → bᵢ ≡ ‘aᵢ (bᵢ ↦ ,)’

fibonacci n return ↦,
	doc “Implements the Fibonacci function.” →
	arg n “The Fibonacci number to compute.” →
	typeof n Int →
	mul a b → m

fibonacci.m

Maybe ↦ class
	Nothing return ↦ return (n j ↦ n)
	Just a return ↦ return (n j ↦ j a)
	nothing n j ↦ n

length a b c return ↦
	mul a a (sa ↦ …)
	mul b b (sb ↦ …)
	mul c c (sc ↦ …)
	add sa sb (t ↦ …)
	add t sc (q ↦ …)
	sqrt q return

length a b c ⇒ sqrt(add(add(mul(a, a), mul(b, b)), mul(c, c)))

length a b c return ↦ mul a a (sa ↦ mul b b (sb ↦ mul c c (sc ↦ add sa sb (t ↦ add t sc (q ↦ sqrt q return)))))


length a b c ⇒ mul a a (sa ↦ mul b b (sb ↦ mul c c (sc ↦ add sa sb (t ↦ add t sc (q ↦ sqrt q return)))))

square a k ↦ mul a a k

square a (mul a a ·)

[aᵢ] ≡ (aᵢ (: ↦ ¿?))

====
if no ‘↦’ in line, but indentation increases then next line is call-line.

fibonacci n return
	doc “Implements the Fibonacci function.”
	arg n “The Fibonacci number to compute.”
	typeof n Int
	mul a b (m ↦,)

FAIL: This is not flexible
====

Definition: *Functional* a procedure is functional if it always calls it's last
argument. This last argument is conventionally called `return`.

TODO: What if the function is not total?

Defintion: *Recursive* a functional procedure is recursive if it can call
itself before calling `return`.

TODO: Can we generalize this to non-functional procedures?

Definition: *Constructor* a function that immediately calls return is a
constructor:

Z return ↦ return (z s ↦ z)
S n return ↦ return (z s ↦ s n)

TODO: Is this definition strong enough?


TODO: Algebraic data types!
TODO: Generators / streams!
TODO: State machines!

