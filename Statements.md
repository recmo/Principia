# Statements

Grammar is in [RFC523][RFC523] standard syntax extended with Unicode Posix pattern syntax. Terminals are codepoints or unicode strings.

[RFC523]: https://tools.ietf.org/html/rfc5234

## Identifiers

Identifiers are either regular identifiers, like `Foo` or operator identifiers, like `×`. An unspaced expression like `Foo×Bar` to is parsed correctly.

```
identifier = regular_identifier / operator_identifier
```

The syntax for regular identifiers conforms to the *Immutable Identifiers* from [Unicode Annex #31 R2][uax31-R2]. This essentially means that anything that could be considered a sensible identifier in any language is recognized as such.

[uax31-R2]: http://www.unicode.org/reports/tr31/#R2

```
regular_identifier = 1*identifier_character
identifier_character = [
  [:^Pattern_White_Space:] &
  [:^Pattern_Syntax:] &
  [:^Noncharacter_Code_Point:] & [
    [:General_Category=Private_Use:]
    [:General_Category=Surrogate:]
    [:General_Category=Control:]
  ]
]
```

TODO: The parser warns when an identifier does not conform to the *Default Identifier Syntax* from [R1][uax31-R1] with the latest version of the Unicode standard.

[uax31-R1]: http://www.unicode.org/reports/tr31/#R1

The parser normalizes identifiers to [Normal Form C][nfc]. Identifiers that are equal after this normalization are *identical*. A further pass compares by identifier caseless matching from [Unicode §3.13 D147][u3], identifiers found equal are *equivalent*. Identifiers that are equivalent, but not identical, are not allowed and will result in a syntax error.

[nfc]: http://www.unicode.org/reports/tr15/#Norm_Forms
[u3]: http://www.unicode.org/versions/latest/ch03.pdf

**GUIDELINE**: Identifiers are short descriptive English phrases with words separated by underscores (`_`). Acronyms are not used, unless they are well-known (like `http`). Identifiers are lower-case except for constructors, which start with a capital letter. Readability is the main goal.

The operator identifiers are symbols such as `+` or `≥`. These are defined by the Unicode [Pattern Syntax][uax31-R3]. The parser will consider them identifier on their own, so `ab+cd` is a sequence of three identifiers: `ab`, `+` and `bc`. Currently the parser treats `+` and other symbolic identifiers as any identifier. Operator identifiers are always a single character.

[uax31-R3]: http://www.unicode.org/reports/tr31/#R3

```
operator_identifier = [:Pattern_Syntax:]
```

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

## String literals

String literals start with a double opening quote (`“`, [U+201C](http://www.fileformat.info/info/unicode/char/2009/index.htm)) and are closed with a double closing quote (`”`,  [U+201D](http://www.fileformat.info/info/unicode/char/2009/index.htm)). Note that the dumb symmetrical typewriter quotes `"` you find on your keyboard have no role.

The parser keeps track of the number of open quotes *within* the literal and only stops the literal when the opening double quote is closed. This has the advantage that to quote a piece of code, you simply enclose it in double quotes:

	hello_world_source ↦ “
		hello_world ↦ “Hello, World!”
	”

There are no escape sequences or ‘here-doc’ syntax. Any Unicode text that has balanced double quotes can be turned into a valid string literal by quoting it. This is basically any sensible string. Strings with unbalanced double quotes can not be literals. They can be create it programmatically.


## TODO: Numeric literals

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


## Complete grammar

```
identifier = regular_identifier / operator_identifier
regular_identifier = 1*identifier_character
identifier_character = [
  [:^Pattern_White_Space:] &
  [:^Pattern_Syntax:] &
  [:^Noncharacter_Code_Point:] & [
    [:General_Category=Private_Use:]
    [:General_Category=Surrogate:]
    [:General_Category=Control:]
  ]
]
operator_identifier = [:Pattern_Syntax:]
```
