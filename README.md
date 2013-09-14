Principia
=========

	hello_world ↦ “Hello, World!”

Futuristic programming, reasoning and proof language.

### About the name
The name is a reference to Whitehead & Russel's [Principia Mathemica](https://en.wikipedia.org/wiki/Principia_Mathematica). ‘Mathematica’ was already taken by another cool programming language, so I choose ‘Principia’. It fits, because this language is focused more on principles and less on mathematics. (Although I envision a fully-proven high performance computer algebra library to be one of the first applications).

Syntax
------

The syntax is minimalist and heavily uses the [Unicode Standard](http://www.unicode.org/versions/Unicode6.2.0/). Please use something like XCompose to access Unicode characters.

The language is a union between a programming language and a proof language. The programming language consists of just two constructs, *calls* and *closures*. It has first-class functions and multiple return values. The proof language has four constructs, *because*, *axiom*, *therefore* and *proofs*. Both these languages live in a space that is similar to a directed cyclic multi-graph.

Unifying these languages is a meta-language that turns these graphs into a linear syntax for consumption by text editors. The meta-language handles scoping and identifier binding. It should be noted that binding is separate from the programming language. The programming language has *no* concept of variables and binding (and therefore, none of the problems associated with it).

### Identifiers


### String literals
String literals start with a double opening quote (‘“’, [U+201C](http://www.fileformat.info/info/unicode/char/2009/index.htm)) and are closed with a double closing quote (‘”’,  [U+201D](http://www.fileformat.info/info/unicode/char/2009/index.htm)). Note that the dumb symmetrical typewriter quotes ‘"’ you find on your keyboard have no role.

The parser keeps track of the number of open quotes *within* the literal and only stops the literal when the opening double quote is closed. This has the advantage that to quote a piece of code, you simply enclose it in double quotes:

	hello_world_source ↦ “
		hello_world ↦ “Hello, World!”
	”

There are no escape sequences. Any Unicode text that has balanced double quotes can be turned into a valid string literal by quoting it. This is basically any sensible string. If you *really* insist on having a string with unbalanced double quotes, you can create it pragmatically.

### Numeric constants
Numeric constants are written with *mantissa*, optional *base* and optional signed *exponent*. If no base is specified it is assumed decimal.

	57 885 161
	5F3759DF₁₆
	10 0110 0110 0101₂
	1₂⁶⁴
	6.626 069 57₁₀⁻³⁴

The mantissa digits are taken from 0-9 and A-Z (uppercase, because Unicode does not recognize [lowercase digits](https://en.wikipedia.org/wiki/Lowercase_digits) anymore). This means the maximum base is 36.
The radix point is given by a full stop dot (‘.’, [U+002E](http://www.fileformat.info/info/unicode/char/002e/index.htm)).
Thin spaces (‘ ’, [U+2009](http://www.fileformat.info/info/unicode/char/2009/index.htm)) can be used to group digits in the mantissa.

The base and exponent are written in decimal using ₀-₉ and ⁰-⁹ respectively. The exponent can have optional signs ⁺ or ⁻.

### Call statement

	output₁ output₂ … ≔ function input₁ input₂ …

### Closure statement

	function input₁ input₂ … ↦ output₁ output₂ …

### Because

	∵ proposition

### Axiom

	⊨ proposition

### Therefore

	∴ proposition

### Proofs

	⊢ proposition

### Inline calls or closures

	… (statement) …

Where the center dot ‘·’ is used as a placeholder to specify which of the outputs of the 

Unless otherwise specified, the expression evaluates to the first outbound edge of the statement.

	map (· x ↦ 

Identifier binding
------------------



Implementation
--------------

* C++11 (Clang)
* Queχ (only if you regenerate the parser)
* LLVM (MCJIT)


How to build and run
--------------------

	git clone git@github.com:Recmo/Principia.git
	cd Principia
	make -j4
	./Principia
