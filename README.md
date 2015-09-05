Stir Language
=============

`Now you're thinking with stacks`

What is Stir?
-------------

Stir is a stack based language currently under design. This project aims to
prototype an interactive interpretter for the language to see if it's feasible.

Rationale
---------

During my formative years I was given an RPN-calculator.  RPN stands for Reverse
Polish Notation and will be explained later in this text. Since then I have
always had an inkling frustration whenever I was forced to use a normal
calculator, and have been meaning to write my own RPN-calculator for a while.
At this point I am competent enough to create a language based on RPN. As such
this will be project that will be useful to me and I anticipate will be fun for
me to develop.

The interpreter
--------------

I'm not a C programmer, but I decided to write the interpretter in C for two
reasons. First I want to learn C properly and there is frankly no other way to
learn than to do, also I get a chance to learn to work with makefiles. Second I
want to see how Stir can perform, and there really is no other way to really see
that other than to just use C, save from assembly (which is not really true,
because I'm sure our friends at GCC can write better assembly from my shit C
code than I could ever write assembly).

TODO
----

The full spec needs to be designed, and subsequently written in some dry and
formulaic way as to not allow for ambiguity. At the end of this document there
is a more detailed TODO. I am too early into the process to really know what I
need, so the TODO is not very extensive.

As to the implementation, you can see under Interpretter that I have merely just
begun with that. I'm looking into using Flex for the grammar, but the spec looks
to be such that a grammar is not strictly speaking required. For completeness, a
grammar would be a good idea (to appease the God of Formality), and Flex seems
like a perfect way to implement that. 

What is RPN?
-----------

RPN stands for Reverse Polish Notation and is also known as postfix notation
([More info](https://en.wikipedia.org/wiki/Reverse_Polish_notation)). Because
RPN was the first term I learned for it, rather than postfix notation, I will
use this term when describing the notation.

In essence operands are pushed onto a stack, while operators pop operands, two
operands for most mathematical operators, off of the stack while pushing the
result of the operation back onto the stack (Hence why it's referred to as stack
based). An example will suffice:

Say we would want to perform the ever useful operation of adding 3 and 4. The
way we would do it in RPN is by pushing the values 3 and 4 onto a stack, and
then applying the plus operator. This can be written like so:

`3 4 +`

The whole operation can be thought of as three operations that each manipulate
the stack by either pushing a value or popping and reading a value. Operands
push, and operators pop. In our previous example, if the operation to be
performed is written on the left and the stack (read from left to right, where
the top of the stack is on the right) is written on the right this can be
illustrated in text thusly:
    
```
3: 3
4: 3 4
+: 7
```

While we at this point hardly recognize the power, or usefullness, of this we
may realize this is only because the example above was trivial. So let's
consider something more useful, for example the formula for all natural numbers,
which in text form can be written like this:

`(n * (n + 1)) / 2`

Two things can be observed here. First, those are a lot of parentheses. Second,
trying to calculate that using a traditional calculator is difficult, even if we
use a calculator with memory registers. Suppose we don't have that, then the
only alternative is to write results down on a piece of paper. However, if we
had an RPN calculator, this would be a breeze to compute. We would do it like
this:

`n 1 + n * 2 /`

To see how this works, let's observe the stack. Because we want to see the
results, let's substitute `n` for (so `5 1 + 5 * 2 /`)

```
5: 5
1: 5 1
+: 6
5: 6 5
*: 30
2: 30 2
/: 15
```

And, of course: `1 + 2 + 3 + 4 + 5 = 15`

If you're still confused, which is to be considered normal at this point, [try
something like this](
https://play.google.com/store/apps/details?id=org.efalk.rpncalc) and experiment,
hopefully only until the command line interactive interpretter is implemented.

The Stir Spec 
-------------

### Design principles


While the details of the language is currently being designed here are the
design principles:

1. The language specs should be minimal. There will be no feature creep. It does
   what it does and nothing more. There will be no "Stir library". Writing a
compiler for it needs to be easy.
2. I should be compilable to assembly. My hopes is that it will be able to rival
   any other native language. From what I speculate the lanugage, as I envision
it to be compiled, should be able to utilize the cache and registers very
efficiently without many optimizations. But, that remains to be seen when I get
there. I want it to be native though.
3. Compilation is evaluation, evaluation is compilation (though not literally).
   From the design that I have currently there is very little difference between
compilation and evaluation, in theory. As I can see it.
4. The language is strongly typed with inferred types and generics is an
   automatic feature of the lanuage. However, the language will support explicit
types. 
5. In addition to being stack based it is also functional.

Does are the basic ideas that I will design the language around.

### More concrete

The input text (the source) is read linearly. There is no need for multiple
passes of the source. The language is well able to be compiled while reading the
source (I believe it is technically context-free, you can tell I'm no expert
here). Compilation involves reading tokens and pushing them onto the program
queue. The program queue are a queue of tokens that will be pushed onto the
stack when the program is evaluated. Tokens are mostly pushed with whitespace,
although some tokens can push themselves, as well as cause others to be pushed
(parentheses for example). The basic tokens are literals and mathematical
operators (including boolean operators and bit manipulator operators). Literals
are integer numbers, decimal numbers, boolean values, strings and characters
(the usual literals). Note that there is no "Number" type, like most scripting
lanugage (Number is an awful type to have in a language). Operators are also the
typical, and are written as they are typically (or at least intuitively).

So for example the string:

`43 34 2 + / 234 and`

...is so far a valid string, according to the description above. Additionally it
will push 7 tokens on the program queue. Because compilation and evaluation is the same
thing, operators do not "pop" the stack (more on that later). Of course there
are more rules than this. So what let's introduce them.

Each operation (with an operation being defined as applying a token from the
queue to the stack) is defined by how it manipulates the stack. There are two
ways of manipulating the stack that is relevant here. First, an operation can
push one or more values onto the stack. Second, an operation can pop one or more
calues onto the stack. Operation can also do both of those things. The notation
for writing what an operation does to the stack looks like this:

`T: a -> b`

Which is read as "if the stack looks like `a` before `T` is applied, the stack
will look like `b` after `T` is applied". `a` and `b` may refer to what happens
to the types, or what happens the values (or anything else, in theory). So, for
+:

```
+: int int -> int
+: a b -> {b+a}
```

We use curly braces in the notation to refer to a value expressed as a
mathemical expression like that, because the other brackets are reserved by the
language.

#### Units

A `unit` is a list of tokens that form a logical unit. Units are encased in
parentheses: `( )`. Units form an identifier scope and they enforce this rule: 

`(b): a -> b`

Meaning that any operations performed in a unit may only pop one stack element
(in addition it may also never pop above that element, under any circumstances)
and MUST push one stack element back onto the stack. It may, in theory, do
nothing at all, in which case it is the `null unit` (literally `()`) which acts
as a `stack guard`. A null unit is how we are able to extend the stack. We are
still not able to pop above it, but the next unit does not the, as input, the
stack before the null unit, and as such can extend the stack with one element.
Note that the operations within the unit encasing all three (the first unit, the
null unit and the second unit) must pop two values for the encasing unit to
follow the rules. This can be expressed as:

```
(): a -> a
(b) () (c): a -> b c
( (b) () (c) ): a -> d
```


A program form its own implicit unit, meaning that a program of Stir takes
excactly one input and returns excactly one output. Of course, the input may be
an empty tuple and the output an arbitrarily sized tuple (and vice versa ad
infinitum) but one input and one output non-the-less.

An (non-complete) example from the previous example of sumation of natural numbers:

`(((5 1 +) 5 *) 2 /)`

Here we use an abundance of parentheses to show that unit can be recursive, i.e.
a unit can have one or many 'child' units. It also shows that we can use units
to make our intentionc clearer to the (human) reader. Of course, this many
parentheses hardly helps. This is also not strictly speaking legal Stir, since
the outer-most unit is a unit that takes no input (or at least does nothing with
the input) but returns an output. We can't have that, that would unbalance the
stack! Before we know it we have stacks running around our backyard. The point
of a program is that it takes input, if we have no input we have no program. The
correct way to write that code is as follows:

`(5 ((1 +) *) 2 /)`

The unit evaluates to two queue positions (queue pointers), the position of the
open paren and the position of the closed paren. These two queue pointers form
what is effectively an anonymous function. When the program queue is evaluated,
the tokens encased in the unti does not get pushed to the stack, only the unit
itself (the two queue pointers). The unit gets evaluated as operators that are
pushed to the stack need to evaluate them. 

#### Identifiers

We actually don't need variables to do anything useful. However they make it
easier for the programmer. In theory any computing at all (as far as I'm aware,
so correct me if I'm wrong) can be done with just manipulating a stack like this
and never storing any value at all in an identifier. Of course, doing so would
make it unreadable, since it's very difficult to express intention. In Stir the
':'-operator is used for assigment.  It's called the pull operator and what it
does is it pulls in the value at the top of the queue into an identifier. The
pull-operator pops the value it stores off of the stack, it can therefore be
used to just pop the stack, which we will see. The identifier is always defined
when pulling in a value, and any previous identifier is either destroyed before
(in the case that the pulls are in the same scope [unit]) or a similarily named
identifier becomes invisible until the newly defined identifier goes out of
scope.

Remember that the value of a unit are the two queue locations encasing the unit.
This is how anonymous functions are named in Stir. 

```
(a b +) :foo
```

here `foo` stores the unit before it, which consists of the queue location of
the open paren and the queue locations of the closed paren. In essence, the
"value" of `foo` is the unit. Note that `a` and `b` appear to be identifiers
themselves. We will expand on that. Note also that there is a whitespace between
the close paren and the colon. This is a requirment for readability. 

Note that identifiers can also hold single values, rather than units. We define these
simply like this:

```
42 :a
```

The value of a is then simply 42. Should we assign another identifier to a
instead, the value of that identifier is stored instead:

```
42 :foo foo :bar
```

Both foo and bar now have the value 42. 
 
Stir is a strongly typed language with inferred types. This means that the
identifier must have a type. In the case of value assigment this is simple, the
type is simply the type of the value. For units it's a little trickier, since
units have an input and an output. So units have two values, the in-value and
the out-value. Identifiers that store units have same kind of type: the in-value
of the unit it stores and the out-value of the unit it stores.

Writing the name of the identifer is an evaluation of that identifier. We write
to the stack the tokens, in order, that are between the two addresses the
identifier stores. In order to call foo on two numbers, effectively adding them,
we could then do something like this:

```
(:a :b a b +) :foo
40 2 foo
```

Now, we already know that that is not good enough. However it makes sense. Store
into `a` and `b` what are effectively the first top values on the stack, then
evaluate them (which pushes their values back onto the stack) and apply the
`+`-operator to those values. That is at least effectively what we want to do.
But how do we do that?

#### Tuples

In order to handle more than one value at a time, passing them in and out of a
unit, we have tuples. Tuples use squared brackets rather than parentheses and
the tokens, or values, are whitespace separated. They look like this:

`[1 2 3]`

The above is a tuple of 3 integers. They are considered one stack element and
as such can the output and/or input of a unit. Unpacking tuples into idenfiers
is done by writing a colon in front of the tuple, this is understood as "pull
into tuple of...", so for example:

`[42 9001] :[foo bar]`

Assigns the value 42 to foo and 9001 to bar. The identifiers are assigned in
order and are assigned the address for the value they are assigned to up until
the next value. The evaluation of the identifier then becomes the value they are
assigned to. We must have at least one whitespace between the first
tuple and the colon, but the second tuple must be followed immediately by the
colon, as `:[` is considered one token. This is true for `:identifier` also.
This is the only way to use the values of tuples separetely, since the tuple is
considered a single value. Note that we can do something with the values of this
tuple and then "return" the original tuple again. Tuples are strongly typed, and 
and may only be unpacked using the same number of identifers, who will get the
same type as the value they are assigned (or rather, what type the value
evaluates to).

A tuple may contain units, or identifers. Like so:

`42 :b [(0 1 -) b]`

This will store -1 and 42 in a tuple, in order.

Now we're ready to really do functions.

`(:[a b] a b +) :foo`  

Of course that is unreadable, so we will do this:

```
( :[a b]
    a b +
) :foo
```

Which, while slightly better, is still rather unreadable. But, that is the
language. Calling `foo` is then done like thus:

```
[40 2] foo
```

Let's implement a more intresting function. Let's, for (a dull) example, implement the
fibonachi function.

#### Conditionals

The if statement is an operator that pops the top value off of the stack and and
temporarily stores it. It then pops and evaluates the next top of the stack. The
type of that value must be simply boolean. If the value is true, the value that
was popped earlier is evaluated. If false, the stored value is simply
disregarded. Remember that the value of a unit are the two queue pointers that
encase the units. If the first value was a unit, the unit gets evaluated. This
is nothing unique, and is the case for all operators operating on units. Here's
an example:


```
(1 2 >)
    (a b +)
if
```

Here we also arrive at our first non-communitive operator, greater-than. What
this means is simply that the order or operations matter, in our case the value
of `1 > 2` is vastly different than `2 > 1`. Typically in RPN operators evaluate
their values in the order they are written, so `1 2 >` means `1 > 2`. Which is
helpful because otherwise it would be (needlessly) confusing. This also means
that `a b +` will be evaluated. If that happens, then the value of that
evaluation is pushed onto the stack. This creates a problem, because it means
the stack has a different length after an `if` depending on what the condition
for the `if` evaluates to. Therefore, an `if` will push an empty unit onto the
stack, should it resolve to false.

In addition, `if` pushes the value of the condition onto the stack. Stir uses
this to implement `else`. `else` simply is actually a selectively ternary
operator and in many ways the opposite of `if`: it pops and stores the top of
the stack. Then pops and evaluates the next top of the stack. If it is false it
evaluates the stored value, otherwise it just disregards it. If it evaluates the
stored value, it will also pop the third item on the stack before that. Why will
be clear in a minute.

So and `if..else`-statement in Stir looks like this:

```
(a b >) 
    (a b -)
if
    (b a -) 
else
```

Think about how the stack will look after this statement. The two units that
actually does some work will push their out value onto the stack. After the if
the stack will looks like this: `{a-b} true`, assuming a is greater than b. Then
we will push the next value in the queue onto the stack, which is the unit
between the if and the else. Then the stack will look like this: `{a-b} true {(b
a -)}`. The else wil pop the first value, see that the next value is true and
then just push true (without evaluating the first value it pops). The stack will
then look like this: `{a-b} true`. Let's now think about the other alternative,
that a is not greater than b. 

After the if, the stack instead will look like this: `() false`. Note that
because the unit was not evaluated, nothing was pushed onto the stack The next
unit will then be pushed onto the stack: `() false {(b a -)}`. Because the
second value is false the `else` will evaluated the first unit. If `else` did
not now also pop the third of the stack, before the evaluation of the first
unit, that would mean the stack would look like this after: `() {b-a} false`.
This creates a problem: we now have 3 items on the stack, and we are not sure
which one holds that value that the `if..else`-condition evaluated. This is a
problem because the stack will be longer the more interoperating conditions we
have, and we have no way to use the value that those conditions generated (which
would be very practical). So, `else` is defined the way it is so that we will
always end up with two values on the stack, where the first is the condition and
the second is the value generated. Now think about this: we have two values on
the stack, but nothing meaningful to do with them to make them into one value.
That is a problem indeed, because practically we really do want to make them
into one value, the if..else-statement is very likely part of a unit, and we are
very much intrested in making the unit only output one value. We could use the
pull-operator for this, and simply not supply an identifer name. Doing this
means we pull the value off of the stack into the void, basically a pop
operator:

```
(a b >)
    (a b -)
if
    (b a -)
else :
:diff
```

Here the identifier diff will have the value of the difference between a and b
expressed as an absolute value. After this the stack is excaclty empty.

Not also that we can do cascading `if..else` in Stir, but it is a litte tricky:


## TODO

Conditionals are hairy. What we want to do is have a complete conditional
statement that takes in one thing, and ouputs whatever it ended up evaluating.
This is, however, not excactly practical, since any conditionals nested in them
will add to the stack. What we really want is and end statement, that cleans it
all up, in the end. I will study what other stack based languages have done.

I need to decide what order non-commutative operators apply the stack. Do we
write `1 2 -` or `2 1 -` to perform `1 - 2`? These are the important questions
in life.

Moreso I want to finish a prototype of an interactive interpreter (I've started
it under the `interpretter` directory). That's the first step in terms of
concrete results. 

Look over the "complilation is evaluation, evaluation is compilation". I know
what that means, but it's hard to put into words what it means.
