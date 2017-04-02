# Scopes and Modules

## Scoping

Identifier scoping, binding, collision, importing and resolving.

Statements can be scoped by

```
scoping_statement₁
	statement₁
	statement₂
	scoping_statement₂
		statement₃
		statement₄
			⋱
	statement₅
	⋮
```


```
… (out₁ · out₃  … statement_kind (a ↦ b) in₂ …) (≔ f out₃) out₁ …
```

```
… dummy d2 out₁ …
	out₁ dummy out₃  … statement_kind tmp in₂ …
		tmp a ↦ b
	d2 ≔ f out₃
```

```
func out₁ out₂ ↦ in₁ in₂
	<body>
```

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


## Explicit scope resolution

**TODO**: https://en.wikipedia.org/wiki/Scope_resolution_operator

Use `∷` or `.` ?

```
...outer_scope.inner_scope.object
.inner_scope.object
object
```

## Modules

https://www.haskell.org/tutorial/modules.html


```
MODULE HelloWorld EXPORTS Main;
IMPORT IO;
BEGIN
  IO.Put("Hello World\n")
END HelloWorld.
```

```
MODULE HelloWorld EXPORTS Main;
FROM IO IMPORT Put;
BEGIN
  Put("Hello World\n")
END HelloWorld.
```


```
Module X
```

## Inline statements

A statement enclosed in parentheses can be used in place of an inbound. The syntax is:

	… (out₁ · out₃  … statement_kind in₁ in₂ …) …

Where the middle dot (`·`, [U+00B7](http://www.fileformat.info/info/unicode/char/b7/index.htm)) is used as a placeholder to specify which inbound or outbound represents the expression in the parent expression.

If there is no middle dot, then it is implicitly prepended, so

	… (out₂ … statement_kind in₁ in₂ …) …

is equivalent to

	… (· out₂ … statement_kind in₁ in₂ …) …

For the purposes of binding identifiers, the inline statement is considered to be in the scope of the parent statement.

### Binding rules (advanced)

**To be written**

Generally tries to be intuitive, with both forward and backward searching.

### Syntax extensions (proposal)

Similar to Coq's [Notation mechanism](https://coq.inria.fr/refman/Reference-Manual014.html)

	[ (x) , … , (y) ]

is parsed as:

	s₁ s₂ x s₃ s₄ … s₄ s₂ y s₃ s₅

with

	s₁ → [
	s₂ → (
	s₃ → )
	s₄ → ,
	s₅ → ]

Expressions of length 0 … n are parsed as:

	s₁ s₅
	s₁ s₂ x₁ s₃ s₅
	s₁ s₂ x₁ s₃ s₄ s₂ x₂ s₃ s₅
	s₁ s₂ x₁ s₃ s₄ s₂ x₂ s₃ s₄ s₂ x₃ s₃ s₅
	⋮

A substitution can be defined as

	(≔ cons x … (≔ cons y nil) … )


**Todo**: Remove `( · ≔ )` and `( · ↦ )` notation from the formal language and reimplemented it as a notation extension.

	( o₂ … oᵢ ≔ f i₁ … iᵢ )

which is to be replaced by `t` and the following statement added:

	t o₂ … oᵢ ≔ f i₁ … iᵢ


The case for `↦` is equivalent. If we can capture `≔` and `↦` in a variable we can even do it like this:

	( x₂ … xᵢ )

to be replaced by `t` with the following statement added:

	t x₂ … xᵢ

And similarly for the variation with `·`:

	( x₁ … xᵢ · y₁ … y₂ )

	x₁ … xᵢ t y₁ … y₂

This version neatly captures the variation where an incoming variable is captured.

While this is already quite powerful, likely enough to define a . Al these examples replace a pattern by a single temporary variable. Is there a use-case for patterns that get replaced
