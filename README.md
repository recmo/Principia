Principia
=========

	hello_world ↦ “Hello, World!”

Futuristic programming, reasoning and proof language.

### About the name
The name is a reference to Whitehead & Russel's [Principia Mathemica](https://en.wikipedia.org/wiki/Principia_Mathematica). ‘Mathematica’ was already taken by another cool programming language, so I choose ‘Principia’. It fits, because this language is focused more on principles and less on mathematics. (Although I envision a fully-proven high performance computer algebra library to be one of the first applications).

Syntax
------

The syntax is minimalist and heavily uses the [Unicode Standard](http://www.unicode.org/versions/Unicode6.2.0/). Please use something like [compose keys](https://en.wikipedia.org/wiki/Compose_key) to enter Unicode characters (here's my setup: [XCompile](https://github.com/Recmo/XCompile)).

The language is a union between a programming language and a proof language. The programming language consists of just two constructs, *calls* and *closures*. It has first-class functions and multiple return values. The proof language has four constructs, *because*, *axiom*, *therefore* and *proofs*. Both these languages live in a space that is similar to a directed cyclic multi-graph.

Unifying these languages is a meta-language that turns these graphs into a linear syntax for consumption by text editors. The meta-language handles scoping and identifier binding. It should be noted that binding is separate from the programming language. The programming language has *no* concept of variables and binding (and therefore, none of the problems associated with it).

### Identifiers
The syntax for identifiers conforms to Unicode 6.2.0 [UAX31-C1](http://www.unicode.org/reports/tr31/) revision 17. This essentially means that anything that could be considered a sensible identifier in any language is recognized as such.

In addition to these identifiers there are symbol identifiers such as ‘+’ or ‘≥’. These are defined by the Unicode [Pattern Syntax](http://www.unicode.org/reports/tr31/#Pattern_Syntax). The parser will consider them identifier on their own, so `ab+cd` is a sequence of three identifier: `ab`, `+` and `bc`.

### Statements
The language consists of a list of statements, each followed by a newline (or more precisely, a paragraph separator). Statements are of the general form:

	out₁ out₂ … operator_symbol in₁ in₂ …

Where `out` and `in` are identifiers or literals and `operator_symbol` is one of the six language specific statements symbols:

* ≔ call
* ↦ closure
* ∵ because
* ∴ therefore
* ⊨ axiom
* ⊢ proofs

These are discussed further down. Since these are keywords, they are excluded from the list of usable identifier symbols.

### String literals
String literals start with a double opening quote (‘“’, [U+201C](http://www.fileformat.info/info/unicode/char/2009/index.htm)) and are closed with a double closing quote (‘”’,  [U+201D](http://www.fileformat.info/info/unicode/char/2009/index.htm)). Note that the dumb symmetrical typewriter quotes ‘"’ you find on your keyboard have no role.

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

The mantissa digits are taken from 0—9 and A—Z (uppercase, because Unicode does not recognize [lowercase digits](https://en.wikipedia.org/wiki/Lowercase_digits) anymore). This means the maximum base is 36.
The radix point is given by a full stop dot (‘.’, [U+002E](http://www.fileformat.info/info/unicode/char/002e/index.htm)).
Thin spaces (‘ ’, [U+2009](http://www.fileformat.info/info/unicode/char/2009/index.htm)) can be used to group digits in the mantissa.

The base and exponent are written in decimal using ₀—₉ and ⁰—⁹ respectively. The exponent can have optional signs ⁺ or ⁻.

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

Inbound identifiers are resolved in the following

### Inline statements (advanced)
	… (out₁ · … operator_symbol in₁ in₂ …) …

Where the middle dot (‘·’, [U+00B7](http://www.fileformat.info/info/unicode/char/b7/index.htm)) is used as a placeholder to specify which of the outputs of the inner statement is to be used in the outer statement.

If there is no middle dot, then it is automatically prepended, so

	… (out₂ … operator_symbol in₁ in₂ …) …

is equivalent to

	… (· out₂ … operator_symbol in₁ in₂ …) …

For the purposes of binding identifiers, the inline statement is considered to be in the scope of the parent statement.

**TODO:** Why limit ‘·’ to outputs, what happens if we allow inline statements to be used as *inputs*?

### Binding rules (advanced)



Programming language
--------------------

### Call statement
	output₁ output₂ … ≔ function input₁ input₂ …

### Closure statement
	function input₁ input₂ … ↦ output₁ output₂ …

Proof language
--------------

### Because
	∵ proposition

### Therefore
	∴ proposition

### Axiom
	⊨ proposition

### Proofs
	⊢ proposition


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
