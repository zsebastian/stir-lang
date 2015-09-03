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
( (b) () (c) )`: a -> d
```


A program form its own impicit unit, meaning that a program
of Stir takes excactly one input and returns excactly one output. Of course, the
input may be an empty tuple and the output an arbitrarily sized tuple (and vice
versa ad infinitum) but one input and one output non-the-less.

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


#### Identifiers

That code is legal, and it also suffices. We actually don't need variables to do
anything useful (they are called identifiers, formally, in Stir, but variables are
also fine). In theory any computing at all (as far as I'm aware, so correct me
if I'm wrong) can be done with just manipulating a stack like this and never
storing any value at all in an identifier. Of course, doing so would make it
unreadable. This is what Stir does : ... No really, ':', is an operator. It's
called the pull operator and what it does is it pulls in the value (although
technically it pulls in the queue pointer, which I will go over shortly) into
an identifier. The identifier is always defined when pulling in a value, and any
previous identifier is either destroyed before (in the case that the pulls are
in the same scope [unit]) or a similarily named identifier becomes invisible
until the newly defined identifier goes out of scope.

Technically an identifer holds not the value that it pulls in, but the pointer
to the queue location of the token that it pulls in, as well as the pointer to
its own queue location. Queue locations are what you would probably expect it to
be, in general the index in the queue that holds the token that was pushed. So
each time a token is pushed onto the stack, the index in the queue is stored
with it. This brings us to another rule of the stack operators:

``` 
(: a -> a [b] 

    where a and b are queue locations, and b is the queue location for the
    initial paren token for the unit. The value of b is implied, it is not 
    pushed.

): [b] -> b

    where b is the queue location for the initial paren token for the
    unit.  
```

This is to say, that the queue location for the finalizing paren token of the
unit is the queue location of the initial paren token. We use this fact for
defining functions. The unit 'remembers' the queue location of the inital paren
token. Remember that units take one input and produce one result? These two
rules combined means that the queue location for the input is the queue location
for the initial paren token, which just so happens to be one after the queue
location of the input. It is 'as if' the unit never existed in the first place.
'As if' is an intentional phrasing and means that all the interpretter or compiler
has to do is act 'as if' it did, i.e. produce the same result that it would if
it acctually did. 

```
(a b +) :foo
```

here `foo` stores the queue location of the open paren as well as the location for
itself. In essence, the "value" of `foo` is the unit. Note that `a` and `b`
appear to be identifiers themselves. We will expand on that. Note also that
there is a whitespace between the close paren and the colon. This whitespace
actually does nothing, the input is expected to be something else than
whitespace here, and so the whitespace is ignored.

Note that identifiers can also hold single values, rather than units. We define these
simply like this:

```
42 :a
```

While technically `a` holds the two pointers encasing the token 42, this is an
'as if' case, and an interpreter may store the value 42 in `a` (however it
"stores" that value), because we know that `a` can not possibly hold any other
value, whatever happens to the stacky.

Stir is a strongly typed language with inferred types. This means that the
identifier must have a type. The type of the identifier is the type that would
be on top of the stack after evaluation, according to this rule:

`Ti: i -> Ta`, where Ti is the type of the identifer, i is the evaluation of the
identifer and Ta is the type of the value left on the stack.

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



## TODO

Obviously what I have done to specify are condionals and loops. My first thought
was to implement them as short circuited `and` with two units separeted with a
null unit. Something like:

```
(1 2 >) ()
    (a b +)
if
```

It doesn't quite work, since the second conditional must be evaluated before the
'if' if we want to read the source linearly (I take that to mean that no
look-ahead is required). `if` here could be exchanged for `and` and it would
give the same result, is the idea here. Which is nice. I believe that once I
figure out a way to get conditionals to work loops will fall into place also.
While I could say that all loops are recursion (and allow for tail recursion)
and that's that, I don't want to do that because it's stupid. In all fairness
since we use a stack based language there should never be a need for recursion
(as traditionally is meant with recursion, ie a function calling itself) anyway.
While we're at it I need to decide what order non-commutative operators apply
the stack. Do we write `1 2 -` or `2 1 -` to perform `1 - 2`? These are the
important questions in life.

Moreso I want to finish a prototype of an interactive interpreter (I've started
it under the `interpretter` directory). That's the first step in terms of
concrete results. 

Look over the "complilation is evaluation, evaluation is compilation". I know
what that means, but it's hard to put it to words.
