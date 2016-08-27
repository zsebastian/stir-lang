@builtin "whitespace.ne" # `_` means arbitrary amount of whitespace

program -> block | expression
block -> "(" expression ")"
expression ->  operand | (operand _ expression _ binop)
binop -> "+" | "-" | "/" | "*"
operand -> number | block 
number -> digit | (natural digits)
natural -> [1-9]
digits -> digit:*
digit -> natural | [0]
