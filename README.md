Principia
=========

	hello_world ↦ “Hello, World!”

Futuristic programming, reasoning and proof language.

### About the name
The name is a reference to Whitehead & Russel's [Principia Mathemica](https://en.wikipedia.org/wiki/Principia_Mathematica). ‘Mathematica’ was already taken by another cool programming language, so I choose ‘Principia’. It fits, because this language is focused more on principles and less on mathematics. (Although I envision a fully-proven high performance computer algebra library to be one of the first applications).

Syntax
------

The syntax is minimalist and heavily uses the [Unicode Standard](http://www.unicode.org/versions/Unicode6.2.0/). Please use something like [compose keys](https://en.wikipedia.org/wiki/Compose_key) to enter Unicode characters (here's my setup: [XCompile](https://github.com/Recmo/XCompile)).

The language is a union between a programming language and a proof language. The programming language consists of just two constructs, *calls* and *closures*. It has eager evaluation, first-class functions and multiple return values. The proof language has four constructs, *because*, *axiom*, *therefore* and *proofs*. Both these languages live in a space that is similar to a directed cyclic multi-graph.

Unifying these languages is a meta-language that turns these graphs into a linear syntax for consumption by text editors. The meta-language handles scoping and identifier binding. It should be noted that binding is separate from the programming language. The programming language has *no* concept of variables and binding (and therefore, none of the problems associated with it).

### Identifiers
The syntax for identifiers conforms to Unicode 6.2.0 [UAX31-C1](http://www.unicode.org/reports/tr31/) revision 17. This essentially means that anything that could be considered a sensible identifier in any language is recognized as such.

In addition to these identifiers there are symbol identifiers such as `+` or `≥`. These are defined by the Unicode [Pattern Syntax](http://www.unicode.org/reports/tr31/#Pattern_Syntax). The parser will consider them identifier on their own, so `ab+cd` is a sequence of three identifiers: `ab`, `+` and `bc`.

### Statements
The language consists of a list of statements, each followed by a newline (or more precisely, a paragraph separator). Statements are of the general form:

	outbound₁ outbound₂ … statement_kind inbound₁ inbound₂ …

Where `outbound` are identifiers and `inbound` can be both identifiers, literals and expressions. The `statement_kind` is one of six specific symbols:

* `≔` call
* `↦` closure
* `∵` because
* `∴` therefore
* `⊨` axiom
* `⊢` proofs

These are discussed further down. Since these are keywords, they are excluded from the list of usable identifier symbols. They are the only keywords in the language.

### String literals
String literals start with a double opening quote (`“`, [U+201C](http://www.fileformat.info/info/unicode/char/2009/index.htm)) and are closed with a double closing quote (`”`,  [U+201D](http://www.fileformat.info/info/unicode/char/2009/index.htm)). Note that the dumb symmetrical typewriter quotes `"` you find on your keyboard have no role.

The parser keeps track of the number of open quotes *within* the literal and only stops the literal when the opening double quote is closed. This has the advantage that to quote a piece of code, you simply enclose it in double quotes:

	hello_world_source ↦ “
		hello_world ↦ “Hello, World!”
	”

There are no escape sequences. Any Unicode text that has balanced double quotes can be turned into a valid string literal by quoting it. This is basically any sensible string. If you *really* insist on having a string with unbalanced double quotes, you can create it programmatically.

### Numeric constants
Numeric constants are written with *mantissa*, optional *base* and optional signed *exponent*. If no base is specified it is assumed decimal.

	57 885 161
	5F3759DF₁₆
	10 0110 0110 0101₂
	1₂⁶⁴
	6.626 069 57₁₀⁻³⁴

The mantissa digits are taken from `0`—`9` and `A`—`Z` (uppercase, because Unicode does not recognize [lowercase digits](https://en.wikipedia.org/wiki/Lowercase_digits)). This means the maximum base is 36.
The radix point is given by a full stop dot (`.`, [U+002E](http://www.fileformat.info/info/unicode/char/002e/index.htm)).
Thin spaces (` `, [U+2009](http://www.fileformat.info/info/unicode/char/2009/index.htm)) can be used to group digits in the mantissa.

The base and exponent are written in decimal using `₀`—`₉` and `⁰`—`⁹` respectively. The exponent can have optional signs `⁺` or `⁻`. Unicode codepoints [U+2080](http://www.fileformat.info/info/unicode/char/2080/index.htm)—[U+2089](http://www.fileformat.info/info/unicode/char/2089/index.htm), [U+2070](http://www.fileformat.info/info/unicode/char/2070/index.htm)—[U+2079](http://www.fileformat.info/info/unicode/char/2079/index.htm) and [U+207A](http://www.fileformat.info/info/unicode/char/207A/index.htm), [U+207B](http://www.fileformat.info/info/unicode/char/207B/index.htm) respectively.

### Scoped statements (advanced)

Statements can be scoped by 

	scoping_statement₁
		statement₁
		statement₂
		scoping_statement₂
			statement₃
			statement₄
				⋱
		statement₅
		⋮


	… (out₁ · out₃  … statement_kind (a ↦ b) in₂ …) (≔ f out₃) out₁ …

	… dummy d2 out₁ …
		out₁ dummy out₃  … statement_kind tmp in₂ …
			tmp a ↦ b
		d2 ≔ f out₃



	func out₁ out₂ ↦ in₁ in₂
		<body>

	→func
	func out₁ out₂ ↦ in₁ in₂
		→out₁
		→out₂
		<body>
		←in₁
		←in₂



	out₁ out₂ ≔ func in₁ in₂
		<body>

	out₁ out₂ ≔ func in₁ in₂
		<body>
		←func
		←in₁
		←in₂
	→out₁
	→out₂

- If statement_kind₁ is a ↦, out₁ is visible to the root scope, all are visible
  in the nested scope. in₁ and in₂ are resolved at the end of the nested scope.
- If statement_kind₁ is a ≔, out₁ is visible to the root scope

Resolution:

- Look up in the scope
- Look down in the scope
- Go up one scope
- Repeat



### Inline statements (advanced)

A statement enclosed in parentheses can be used in place of an inbound. The syntax is:

	… (out₁ · out₃  … statement_kind in₁ in₂ …) …

Where the middle dot (`·`, [U+00B7](http://www.fileformat.info/info/unicode/char/b7/index.htm)) is used as a placeholder to specify which of the outbounds of the inner statement is to be used at that place in the outer statement.

If there is no middle dot, then it is implicitly prepended, so

	… (out₂ … statement_kind in₁ in₂ …) …

is equivalent to

	… (· out₂ … statement_kind in₁ in₂ …) …

For the purposes of binding identifiers, the inline statement is considered to be in the scope of the parent statement.

**TODO:** Why limit `·` to outbounds, what happens if we allow inline statements to be used as *inbounds*?

**TODO:** We can make it even more consise by defining […] and {…} to mean
(≔ …) and (↦ …) respectively. Than we can write

funcs k ↦ a b
	a n ↦ (≔(≔ if n (↦ (≔ b (≔ sub n k))) (↦ 2)))
	b n ↦ (≔(≔ if n (↦ (≔ a (≔ sub n k))) (↦ 3)))

as

	funcs k ↦ a b
		a n ↦ [[if n {[b [sub n k]]} {2}]]
		b n ↦ [[if n {[a [sub n k]]} {3}]]


### Binding rules (advanced)

**To be written**

Generally tries to be intuitive, with both forward and backward searching.

Programming language
--------------------

### Call statement
	return₁ return₂ … ≔ function argument₁ argument₂ …

The call statement calls the function in inbound position one with the remaining inbounds as arguments. The return values are assigned to the outbounds.

### Closure statement
	function argument₁ argument₂ … ↦ return₁ return₂ …

The closure statement is perfectly symmetrical to the call statement. Now, the function and it's arguments are outbound and the return values are inbound.

### On eagerness

The language is eager

Proof language
--------------

**To be written**

### Because (pre-conditions)
	∵ proposition

### Therefore (post-conditions)
	∴ proposition

### Axiom
	⊨ proposition

### Proofs (assertions)
	⊢ proposition


Implementation
--------------

* C++11 (Clang)
* Queχ (only if you regenerate the parser)
* LLVM (MCJIT)

The Queχ based parser produces an abstract syntax tree (AST). The binder then binds all the identifiers in the AST. From this the program data flow graph (DFG) is created. The DFG is the natural representation of the program and is where static verification and optimizations like constant propagation happen.

From this DFG the (nested) closures are extracted and lambda lifted. The lifted closures are then serialized and converted to a simple stack machine language. The stack machine language is compiled to LLVM intermediate representation (IR). The IR run through optimization passes and JIT compiled to native machine code.

How to build and run
--------------------

	git clone git@github.com:Recmo/Principia.git
	cd Principia
	make -j4
	./Principia Ackermann.txt PRA 3 3
