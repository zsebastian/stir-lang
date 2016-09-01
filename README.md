Stir Language
=============

`Now you're thinking with stacks`

What is Stir?
-------------

Stir is a stack based language currently under design. It is functional and
without side effects. This project aims to prototype an interactive interpretter
for the language to see if it's feasible.

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

Also, edit this text. The text is a mess.

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
consider something more useful, for example the formula adding all natural
numbers up to `n`, which in text form can be written like this:

`(n * (n + 1)) / 2`

Two things can be observed here. First, those are a lot of parentheses. Second,
trying to calculate that using a traditional calculator is difficult, even if we
use a calculator with memory registers. Suppose we don't have that, then the
only alternative is to write results down on a piece of paper. However, if we
had an RPN calculator, this would be a breeze to compute. We would do it like
this:

`n 1 + n * 2 /`

To see how this works, let's observe the stack. Because we want to see the
results, let's substitute `n` for `5` (so `5 1 + 5 * 2 /`)

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
compilation and evaluation, in theory. As I can see it. I will expand on this,
but I feel that it is crucial (though hard for me to explain).
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
are integer numbers, floating point numbers, boolean values, strings and characters
(the usual literals). Note that there is no "Number" type, like most scripting
lanugage (Number is an awful type to have in a language). Operators are also the
typical, and are written as they are typically (or at least intuitively).

So for example the string:

`43 34 2 + / 234 and`

...is so far a valid string, according to the description above. Additionally it
will push 7 tokens on the program queue. Because compilation and evaluation is the same
thing, operators do not "pop" the stack (more on that later). Of course there
are more rules than this. So let's introduce them.

Each operation (with an operation being defined as applying a token from the
queue to the stack) is defined by how it manipulates the stack. There are two
ways of manipulating the stack that is relevant here. First, an operation can
push one or more values onto the stack. Second, an operation can pop one or more
values onto the stack. Operations can also do both of those things. The notation
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
parentheses: `( )`. Units are meant to enhance predicability and provability of
a program. They make sure the stack behaves in specific ways. There are many
types of units, each with their own behaviour. The first unit is the basic unit:

### Basic unit

The basic unit simply forms a adds a value onto the stack.

`( expr{a} ) -> a`

Meaning that any operations performed in a unit may only push one stack element.
In fact it must do so. In addition it will never pop above it's left
parenthesis. There is no way to feed a value into a unit. The type of the unit
is simply the type that is pushed onto the stack at the end of it.

### Lambda unit

A lamda unit is a unit that does pop above it, but may only pop one element. A
pull unit also pushes one element onto the stack. They are rougly equivalent to
anonymous functions in other languages. The lambda unit is a unit with
a pull operator (':') attached to the left and right parenthesis:

`a :( expr ): -> expr{a}

A program form its own implicit lamda unit, meaning that a program of Stir takes
excactly one input and returns excactly one output. Of course, the input may be
an empty tuple and the output an arbitrarily sized tuple (and vice versa ad
infinitum) but one input and one output non-the-less. (We will deal with tuples
and other collections later).

Lambda units are not executed when they are read. They must be invoked. A lambda
unit may be stored into an identifier by attaching an identifier name
to the right pull operator. Here is an example:

#Store into an identifer and then invoke:
```
:(1 +):addOne
1 addOne
```

Note that an identifier doesn't have to be attached, at least not yet. The lamda
unit will then be pushed onto the stack as a sort of function pointer. It can
then be stored later into an identifer, using the pull operator again:

```
:(1 +): (1 1 +) :two :addOne

```

This is a contrived example, but this mechanism is useful. Note that `:(1
+):addOne` is simply shorthand for `:(1 +): :addOne`.



### Consumer unit and Producers unit

There are no loops in Stir. Instead, stir bases its looping completely in
monoids (right?). If you are familiar with .net's LINQ you will understand these
units. Consumer/Producers are linked with a stream (similar to IEnumerable in
LINQ, but isn't). A producer takes some input and yield some output to the
consumer. The consumer may in turn be a producers also, and yield output to
another consumer, and so on. Typically the first consumer in a chain accepts
some tuple that it can unzip and start yielding the elements as a producer down
to the next consumer. The last consumer will have some transformation on the
original tuple (that is now a stream), which it can then zip back up again into
a new, and transformed, tuple.

We will delve more into consumer/producer units at a later time, but syntax is
as follows:

>( * 2 )>

In the above example, the tuple coming in is unzipped and transformed by
multiplying each value by 2, then zipped back up into a new tuple as output.
Should the next token be another consumer, the output is not zipped up, but
instead streamed into that consumer, for example:

>( * 2 )>( 1 + )>

A calling program may act as a producer, if so the first token must be a
consumer unit. The program may act as a producer for the calling program also,
if the last token is a producer unit. Programs such as these can take in lines
from, for example, stdin and push out results from stdout. The stir runtime can
accept any number of inputs and output. A stir program may, for example, consume 
HTTP requests and produce responses. Or it may consume HTTP requests and produce
database entries (We will talk more about strings later).

Again, we will delve more into consumers and producers, as this is one of the
main features of stir. Stir is a programming language for data processing, more
than anything else.

#### Identifiers

We actually don't need variables to do anything useful (they are called
identifiers, formally, in Stir, but variables are also fine). In theory any
computing at all (as far as I'm aware, so correct me if I'm wrong) can be done
with just manipulating a stack like this and never storing any value at all in
an identifier. However, because we don't like unreadable code stir supports
identifiers. Identifiers are defined by the pull operator and what it does
is it pulls in a value into an identifier. 

We touched upon identifiers earlier with lamda units. Note that identifiers can
also hold single values, rather than lamda units. We define these simply like
this:

```
42 :a
```

Stir is a strongly typed language with inferred types. This means that the
identifier must have a type. The type of the identifier is the type that would
be on top of the stack after evaluation, according to this rule:

`Ti: i -> Ta`, where Ti is the type of the identifer, i is the evaluation of the
identifer and Ta is the type of the value left on the stack.

Writing the name of the identifer is an evaluation of that identifier. Here's an
example to showcase many possible uses of identifiers:

```
1 :a :( :b a b +): :foo 
40 foo
```

This will require some thinking to understand. Let's over it. Let me write the
stack in brackets besides the token we operate on. Let left be the top of the
stack.

        []      // Start with an empty stack
1       [1]
:a      []      // Declaring an identifier pops the stack and store it into the identifier
                // hence "pull operator"
:(      [*]     // We push the address of the the operator onto the stack here.

):      [*]     // Note that nothing in the brackets is executed, and nothing new
                //is pushed
:foo    []      //Store the address in foo
40      [40]    
foo     [40]    // What happens now is the instruction pointer is set to the ':('

:b      []      // Stored 40 in b
a       [1]     // a is in scope here and has the value 1. We talk about scope
                // later
b       [1 40]
\+       [41]   

):              // We jump back to where we were before we invoked foo.


Now, we already know that that is not good enough. However it makes sense. Store
into `a` and `b` what are effectively the first top values on the stack, then
evaluate them (which pushes their values back onto the stack) and apply the
`+`-operator to those values. That is at least effectively what we want to do.
But how do we do that?

#### Tuples

In order to handle more than one value at a time, passing them in and out of a
unit, we have tuples. (Note that, then, built in operators are the only
operations that may pop more than one value from the stack with one operation).
Tuples use squared brackets rather than parentheses and the tokens, or values,
are whitespace separated. They look like this:

`[1 2 3]`

The above is a tuple of 3 integers. They are considered one stack element and
as such can be the output and/or input of a unit. Unpacking tuples into idenfiers
is done by writing a colon in front of the tuple, this is understood as "pull
into tuple of...", so for example:

`[42 9001] :[foo bar]`

Assigns the value 42 to foo and 9001 to bar. The identifiers are assigned in
order and are assigned the address for the value they are assigned to up until
the next value. The evaluation of the identifier then becomes the value they are
assigned to. We must have at least one whitespace between the first tuple and
the colon, but the second tuple must be followed immediately by the colon, as
`:[` is considered one token. This is true for `:identifier` also.  This is the
only way to use the values of tuples separetely, since the tuple is considered a
single value. Note that we can do something with the values of this tuple and
then "return" the original tuple again. Tuples are strongly typed, and may only
be unpacked using the same number of identifers, who will get the same type as
the value they are assigned (or rather, what type the value they evaluate to).

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

## Flow control

#### Enumeration and Coroutines

A core feature of the language is based on a producer/consumer concept, and it
is how deal with collections of data. A producer supplies a consumer with
serial data, and can apply a transformation. Let's look at an example

`>(1 +)>`

`>(` denotes a consumer, while `)>` denotes a producer. The unit encased apply a
transformation and yields the value of the transformation to the consumer. As is
typical the consumer only can handle one value from the stack, but that value is
typically (although it doesn't have to be) a collection of some sort. Consumer
and producers together form a coroutine, and they have certain characteristics
that we will go into. We already know about tuples, so let's look at a coroutine
involving a tuple.

```
((:[a] a a *) :exp
  [1 2 3 4 5 6] >(
    exp
  )> :[a b c d e f])
```

Here we take in a tuple with values 1 through 6, apply and a function
exponentiating the value by 2 and store them as values a through f (we can do
better than that, but so far this is what we can do). This is equivilent to the
C# code:

```
int exp(int a)
{
    return a * a;
}

IEnumerable<int> expEnumerable(IEnumerable<int> arr)
{
    foreach(var a in arr)
    {
        yield return exp(a);
    }
}
```

If you are familiar with C#'s enumerables, this is where I take the idea
(although it is an old mathematical idea and is used in functional languages.
They are monads, as far I as understand them). I find them very useful and will
be useful for a language as stir, as I foresee the use cases for stir might be.
It is useful for transforming data.


#### If

Conditionals in the language must satisfy certain properties. Most importantly a
conditional must always leave the stack in the same condition no matter what the
result of the condition is. What that means is that after we have evaluated a
condition we must find the stack the same length and with the same types no
matter what the condition evaluated to. This has certain implications, for
example there can be no naked ifs, and both ifs and elses must push the same
types of values onto the stack, if any. Here we also encounter another problem,
with conditionals in coroutines, me might want to only yield a value if a
certain condition evaluates to true (or false).

Let's look at conditionals as if they were a function. In other programming
languages, if we call a function we expect a return (assuming no void
functions). A function may give us a value if a certain condition is met,
otherwise it may give us some other value (null perhaps). In stir there are no
null values. Null values are the bane of all existance and should be avoided at
all costs, and so in stir there are no null values. So we can't do that. But the
only use of a conditional is just this: produce one value if a condition is met,
otherwise another value. Because we have no side effects in stir, this is the
only reason to ever use conditionals. Stir is always concerned with data
transformation, and nothing more. It always ever takes an input and produces an
output, there is no other type of program in stir. So it makes sense, I think,
to restrict conditionals this way. While there are ways to effectively do other
things with conditionals, which we will see once other data structures are
presented, for now this is the only thing we ever want to do. So let's start
with a working example.

```
  (:[a] (a 0 <) (-a) (a) ifelse) :abs # (-a) and (a) needn't be encased in units
```

This defines an absolute value function. `ifelse` is an operator that takes in 3
values: a boolean value, and two other values. If the first value is true, it
will push the second value onto the stack, otherwise it will push the third
value onto the stack. We can do `if... else if... else` like this:

```
  (:[a] 
    ((a 0 <)
      (-1)
      ((a 0 >)
        (1)
        (0)
      elseif)
    elseif)) :unit
```
#### Grammar

The grammar for Stir is simple up until the expression symbol. The
expression symbol is specified as pseudocode instead.

program -> expression
unit -> "(" ws expression ws ")"
operand -> unit | literal | identifier | tuple
tuple -> "[" ws [ expression { ws "," ws expression } ] ws "]"
binaryoperator -> "\*" | "+" | "-" | "/"

literal -> integer | float | double

double -> natural "." { digit }
float -> natural ("f" | "." { digit } "f")
integer -> natural

natural -> zero | nonzerodigit { digit }

zero = "0"
nonzerodigit = [1-9]
digit = [0-9]

identifier = ("\_" | alphabetic) { alphanumeric }

alphanumeric = [0-9a-zA-Z]
alphabetic = [a-zA-Z]

ws -> { whitespace }

# while fairly simple, this can not be expressed in
# any grammar notation that I am aware. It should be 
# possible to implement within a handwritte 
# recursive decent parser
expression ->
    balance = 0
    while there are tokens
        if next is operand
            consume
            balance++
        else if next is ternaryoperator
            consume
            balance-=2
        else if next is binaryoperator
            consume
            balance--
        else if next is unaryoperator
            consume
        else if next is voidoperator #this is the pull operator
            consume
            balance--
        else if next is at least one whitespace
            consume
        else
            break from loop
    
    if balance < 1
        error "too many operators"
    else if balance > 1
        error "too few operators" 

## TODO

Implement:
    - identifiers
    - units
    - tuples
    - floating point
    - different sizes for signed/unsigned datatypes
    - literal-suffixes (l, ul, u, i, b, i32, i8, u32 etc)
    - strings
Parser

Virtual machine
    - start with a simply bytecode to get things going 

Selective yielding in producer, yield if (some expression), also yield many from
one value.


