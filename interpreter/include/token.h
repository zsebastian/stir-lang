#ifndef TOKEN_H_
#define TOKEN_H_
#include <assert.h>

typedef enum TokenType
{
    TOK_NONE = -1,
    TOK_PUSH_BASIC_UNIT,
    TOK_POP_BASIC_UNIT,
    TOK_PUSH_LAMBDA_UNIT,
    TOK_POP_LAMBDA_UNIT,
    TOK_ADDITION,
    TOK_SUBTRACTION,
    TOK_DIVISION,
    TOK_MULTIPLICATION,
    TOK_PULL_OPERATOR,

    TOK_INTEGER,

    TOK_IDENTIFIER,
} token_type_t;

typedef struct token
{
    token_type_t type;
    char **full_string;
    int index, length;
} token_t;

static inline int token_is_operator(token_type_t token)
{
    assert(token != TOK_NONE);
    switch(token)
    {
        case TOK_ADDITION:
        case TOK_SUBTRACTION:
        case TOK_DIVISION:
        case TOK_MULTIPLICATION:
        case TOK_PUSH_BASIC_UNIT:
        case TOK_POP_BASIC_UNIT:
        case TOK_PULL_OPERATOR:
            return 1;
        default:
            return 0;
    }
}

static inline int token_is_operand(token_type_t token)
{
    return !token_is_operator(token);
}

#endif
