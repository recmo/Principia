# Continuation-Passing Style

Function call:

	f arg₁ arg₂ … continuation₁ continuation₂ …

(There is no difference between arguments and continuations)

The order of the arguments in the function call is syntactically significant,
but semantically it is just a mapping from symbols on the call-site to symbols
on the closure site.

Closure:

	λ f arg₁ arg₂ … continuation₁ continuation₂ …

The `λ` is what pure syntax and has no meaning other than to distinguish a
string of binding sites from a string of references.

Task: Given a function `add`, create a function `double`.

	λ double x ret
		add x x ret

The first line defines a closure, the second is a function call. How do we
distinguish those in syntax?

Is there something inherent in the syntax that makes in distinguishable? In the
above example we know the second line must be a function call because the symbol
`x` is used twice. A closure will bind it's arguments and they therefore can
only appear once.

	λ double x ret
		add x x ret

Task: given the above `double` make a function `quadruple`.

	λ quadruple x ret
		double x quadruple_next
		λ quadruple_next y
			double y ret

This requires defining a closure.

How do we pass a closure to a function?

Is the `f` a sort of return argument that can be eliminated? Can we make it more
CPS by giving a closure a continuation to which the closure is supposed to be
passed?

# A program is a set of closures and a set of functions

	λ add a b r
	λ double x re
		add x x re
	λ quadruple y ret
		double y quadruple_next
		λ quadruple_next z
			double z ret

closures:
	[add a b r]
	[double x re]
	[quadruple y ret]
	[quadruple_next z]

functions:
	[add x x re]
	[double y quadruple_next]
	[double z ret]

This only works if the closures are bound at compile time — which we do not
want.

# Factorial

	λ is_zero n ret_true ret_false
		# Call `ret_true` if `n = 0` or else call `ret_false`
	
	λ pred n ret
		# Call `ret` with `n - 1`
	
	λ mul n m ret
		# Call `ret` with `n × m`
	
	λ factorial n ret
		is_zero n base recurse
		λ base nil
			ret 1 nil
		λ recurse nil
			pred n recurse' nil
			λ recurse' n'
				factorial n' recurse''
				λ recurse'' n''
					mul n n'' ret


# Double recursion

Let look at an example with more complex recursion. The doubly recursive
even-odd functions:

	λ pred n ret
		# Call `ret` with `n - 1`
	
	λ is_zero n ret_true ret_false
		# Call `ret_true` if `n = 0` or else call `ret_false`
	
	λ even n ret_true ret_false
		is_zero n ret_true recurse
		λ recurse
			pred n recurse'
			λ recurse' n'
				odd n' ret_true ret_false
	
	λ odd n ret_true ret_false
		is_zero n ret_false recurse
		λ recurse
			pred n recurse'
			λ recurse' n'
				even n' ret_true ret_false


Let's make all the identifiers unique

	λ pred pn pret
		# Call `ret` with `n - 1`
	
	λ is_zero in itrue ifalse
		# Call `ret_true` if `n = 0` or else call `ret_false`
	
	λ even en etrue efalse
		is_zero en etrue erec
		λ erec
			pred en erecp
			λ erecp enp
				odd enp etrue efalse
	
	λ odd on otrue ofalse
		is_zero on ofalse orec
		λ orec
			pred on orecp
			λ orecp onp
				even onp otrue ofalse


Now we can safely flatten and sort the program:

	1 pred pn pret
	2 is_zero in itrue ifalse
	3 even en etrue efalse
	4 odd on otrue ofalse
	5 erec
	6 orec
	7 erecp enp
	8 orecp onp

	1 pred en erecp
	2 pred on orecp
	3 is_zero en etrue erec
	4 is_zero on ofalse orec
	5 even onp otrue ofalse
	6 odd enp etrue efalse

There are eight closures, two of which are imports, and six function calls.

We can drop the closures and simply call them by argument number

	1 1.1 3.2 7.1
	2 1.1 4.2 8.1
	3 2.1 3.2 3.3 5.1
	4 2.1 4.2 4.4 6.1
	5 3.1 8.2 4.3 4.4
	6 4.1 7.2 3.3 3.4

To find closures, remove entries ending in `.1` and forget the argument numbers

	1 {3} # Missing 5
	2 {4} # Missing 6
	3 {3}
	4 {4}
	5 {8 4}
	6 {7 3}

We have recovered the function bodies of {3,4,7,8} but we are missing {5,6}!

# Try again!

There is a hack to solve this problem by adding explicit dependencies. If we
implicitly extend argument list with an infinite number of `nil` values, and
have closures ignore addition.

	λ pred n ret
		# Call `ret` with `n - 1`
	
	λ is_zero n ret_true ret_false
		# Call `ret_true` if `n = 0` or else call `ret_false`
	
	λ even n ret_true ret_false
		is_zero n ret_true recurse
		λ recurse nil
			pred n recurse' nil
			λ recurse' n'
				odd n' ret_true ret_false
	
	λ odd n ret_true ret_false
		is_zero n ret_false recurse
		λ recurse nil
			pred n recurse' nil
			λ recurse' n'
				even n' ret_true ret_false


Let's make all the identifiers unique

	λ pred pn pret
		# Call `ret` with `n - 1`
	
	λ is_zero in itrue ifalse
		# Call `ret_true` if `n = 0` or else call `ret_false`
	
	λ even en etrue efalse
		is_zero en etrue erec
		λ erec enil
			pred en erecp enil
			λ erecp enp
				odd enp etrue efalse
	
	λ odd on otrue ofalse
		is_zero on ofalse orec
		λ orec onil
			pred on orecp onil
			λ orecp onp
				even onp otrue ofalse


Now we can safely flatten and sort the program:

	1 pred pn pret
	2 is_zero in itrue ifalse
	3 even en etrue efalse
	4 odd on otrue ofalse
	5 erec enil
	6 orec onil
	7 erecp enp
	8 orecp onp

	1 pred en erecp enil
	2 pred on orecp onil
	3 is_zero en etrue erec
	4 is_zero on ofalse orec
	5 even onp otrue ofalse
	6 odd enp etrue efalse

There are eight closures, two of which are imports, and six function calls.

We can drop the closures and simply call them by argument number

	1 1.1 3.2 7.1 5.2
	2 1.1 4.2 8.1 6.2
	3 2.1 3.2 3.3 5.1
	4 2.1 4.2 4.4 6.1
	5 3.1 8.2 4.3 4.4
	6 4.1 7.2 3.3 3.4

To find closures, remove entries ending in `.1` and forget the argument numbers

	1 {3 5}
	2 {4 6}
	3 {3}
	4 {4}
	5 {8 4}
	6 {7 3}

We have now recovered all the function bodies.

# Scott encoding

Task: Create a type with two constructors, `C₁` taking two parameters and `C₂`
taking no parameters.

	
	λ C₁ x₁ x₂ ret
		λ obj c₁ c₂
			c₁ x₁ x₂
		ret obj
	λ C₂ ret
		λ obj c₁ c₂
			c₂
		ret obj
	

# Scott booleans

Task: Create a type with two constructors, `True` and `False` taking no
parameters. Instantiate two global values.

	
	λ True ret
		ret obj
		λ obj t f
			t
	λ False ret
		ret obj
		λ obj t f
			f
	
	True ret
	λ ret true
		False ret
		λ ret false
			md “We now have `true` and `false`”
	

# Scott graph

TASK: Create a 

# Syntax 

Syntax rules:

* If all symbol names are unique, line order should not matter. Line order is
  only relevant for finding the binding site of symbols.

* Similarly for indentation.


## Parenthesis

Like [de Bruijn indices](https://en.wikipedia.org/wiki/De_Bruijn_index) with
`·` meaning index 1.

TODO: What about higher indices? Let's for now take the progression `·`, `:`,
`∴`, `∷`, …

	
	λ C₁ x₁ x₂ (· (λ · (· x₁ x₂)  _  ))
	λ C₂       (· (λ ·  _        (·) ))
	
	λ True  (· (λ · (·)  _ ))
	λ False (· (λ ·  _  (·)))
	
	True  (λ · true )
	False (λ · false)
	

* http://homepage.divms.uiowa.edu/~astump/papers/archon.pdf

This allows imperative style programming where `f(g(x))` can be written as

	f (g x (λ · :))

## Reflection

The program is it's own encoding, we provide functions to reflect.

TASK: Make a symbolic differentiator taking a function constructed using `add`
and return a function computing it's derivative.

	λ double a ret
		add a a ret
	
	λ diff func ret
		c “Binding site, variable, call, closure”
		… 
	
	diff double (λ · double_da)

In reflection, we should have `λ · x (· x)` equivalent to `λ · y (· y)`.

Wand's system:

	λ fexpr func ret
		“Calls ret with a Morgensen-Scott encoding of func”
	
	λ eval rep ret
		“Calls ret with a lambda of the Morgensen-Scott encoded functions”

`fexpr` is a built-in function! It can probably not be implemented. And if
`eval` can be implemented at all, it will be very difficult.

Archon's system:

`open`, `vcomp`, `swap`, `_ : _ _ _ _ _ _ _`

Other:

`map-closure`


TASK: Make a function `λ itterate f n ret` that calls `ret` with a closure
itterating `f` `n` times.



÷closure


Scott encoding recursive/cyclical data structures? Graphs?


* The first binding site of a closure is rather special, it is as it where a
  handle. It is also the only binding site that has a value outside of the
  function body.
* The first variable of a calls does not have quite the same speciality.


TASK: Take a closure and replace instances of `add` with `mul`.

	λ double a ret
		add a a ret
	
	replace double add mul (λ · square)


	λ replace function from to ret
		…


* Any closure can be represented by a reference to the lambda itself, and a
  reference to the unbound-closure. The environment for `double` is empty, but
  the call to `add` will have `a` and `ret` in it's environment.

## POPLmark

<https://www.seas.upenn.edu/~plclub/poplmark/poplmark.pdf>

* 
* https://www.cs.utexas.edu/~wcook/Drafts/2012/graphs.pdf









