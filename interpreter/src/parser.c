#include "../include/parser.h"
#include "../include/error.h"
#include <regex.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

/* returns a new copy of a that is malloced 
 */
char *str_copy(char *a)
{
    char *n = (char *)malloc(strlen(a));
    strcpy(n, a);
    return n;
}

/* This returns a string that is newly allocated.
 * It leave a and b intact.
 */
char *str_concat(char *a, char* b)
{
    if (!a)
    {
        return str_copy(b);
    }
    if (!b)
    {
        return str_copy(a);
    }

    int al = strlen(a);
    int bl = strlen(b);
    char *n = (char *)malloc(al + bl + 1);
    n[0] = '\0';
    strcat(n, a);
    strcat(n, b);
    return n;
}

int parser_init(parser_t *parser)
{
    kv_init(parser->token_queue);
    parser->string = NULL;

    return ERR_SUCCESS;
}

int parser_free(parser_t *parser)
{
    kv_destroy(parser->token_queue);
    free(parser->string);
    return ERR_SUCCESS;
}

typedef struct parse_job
{
    int next_chr;
    int look_ahead;
    parser_t* parser;
} parse_job_t;

typedef int (*symbol_t)(parse_job_t *);

void
backtrack(parse_job_t* job, int to_token_index)
{
    assert(to_token_index < kv_size(job->parser->token_queue));

    token_t token = kv_A(job->parser->token_queue, to_token_index);
    job->next_chr = token.index;
    job->look_ahead = token.index;
    kv_size(job->parser->token_queue) = to_token_index + 1;
}

void 
consume(parse_job_t* job, token_type_t token_type)
{
    int characters = job->look_ahead - job->next_chr;
    if (token_type != TOK_NONE)
    {
        token_t token;
        token.full_string = &job->parser->string;
        token.type = token_type;
        token.length = characters;
        token.index = job->next_chr;
        kv_push(token_t, job->parser->token_queue, token);
    }
    job->next_chr = job->look_ahead;
}

int one(parse_job_t* job, symbol_t symbol)
{
    int current_token = kv_size(job->parser->token_queue) - 1;
    int ret = symbol(job);
    if(ret == 0)
    {
        backtrack(job, current_token);
    }
    return ret;
}

inline int greedy_repeat(parse_job_t* job, symbol_t symbol)
{
    int i = 0;
    int next;
    while((next = one(job, symbol)) != 0)
        i += next;

    return i;
}

inline int repeat_at_least(parse_job_t* job, symbol_t symbol, int least)
{
    int current_token = kv_size(job->parser->token_queue) - 1;
    int i = 0;
    int next;
    while((next = one(job, symbol)) != 0)
        i += next;
    if (i >= least)
    {
        return i;
    }
    else
    {
        backtrack(job, current_token);
        return 0;
    }
}

inline char read(parse_job_t* job)
{
    return job->parser->string[job->look_ahead];
}

inline char next(parse_job_t* job)
{
    char ret = read(job);
    job->look_ahead++;
    return ret;
}

inline bool
skip_whitespace(parse_job_t* job)
{
    while(read(job) <= 32)
    {
        next(job);
    }
    consume(job, TOK_NONE);
    return true;
}

inline int try_tokenize(parse_job_t* job, symbol_t symbol, token_type_t token)
{
    int ret = one(job, symbol);
    if (ret)
    {
        consume(job, token);
    }
    return ret;
}


int sym_program(parse_job_t* job);
int sym_expression(parse_job_t* job);
int sym_unit(parse_job_t* job);
int sym_operand(parse_job_t* job);
int sym_operator(parse_job_t* job);
int sym_number(parse_job_t *job);

int tok_push_unit(parse_job_t *job);
int tok_pop_unit(parse_job_t *job);
int tok_integer(parse_job_t *job);

int term_natural(parse_job_t *job);
int term_zero(parse_job_t *job);
int term_left_paren(parse_job_t *job);
int term_right_paren(parse_job_t *job);

int sym_program(parse_job_t* job)
{
    return skip_whitespace(job) &&
        (one(job, sym_expression) ||
        one(job, sym_unit)) &&
        skip_whitespace(job);
}

int sym_unit(parse_job_t* job)
{
    return (one(job, tok_push_unit) && 
        skip_whitespace(job) &&
        one(job, sym_expression) && 
        skip_whitespace(job) &&
        one(job, tok_pop_unit));
}

int sym_expression(parse_job_t* job)
{
    return (one(job, sym_operand) && 
        skip_whitespace(job) &&
        one(job, sym_expression) && 
        skip_whitespace(job) &&
        one(job, sym_operator)) ||
        one(job, sym_operand);
}

int sym_operand(parse_job_t* job)
{
    char n = read(job);
    token_type_t t;

    switch(n)
    {
        case '*':
            t = TOK_MULTIPLICATION;
        case '+':
            t = TOK_ADDITION;
        case '-':
            t = TOK_SUBTRACTION;
        case '/':
            t = TOK_DIVISION;
            
            consume(job, t);
            return true;

        default:
            return false;
    }
}

int sym_operator(parse_job_t *job)
{
    return one(job, tok_integer);
}


int tok_push_unit(parse_job_t* job)
{
    return try_tokenize(job, term_left_paren, TOK_PUSH_UNIT);
}

int tok_pop_unit(parse_job_t* job)
{
    return try_tokenize(job, term_right_paren, TOK_POP_UNIT);
}

int tok_integer(parse_job_t* job)
{
    return try_tokenize(job, sym_number, TOK_INTEGER);
}

int sym_number(parse_job_t *job)
{
    return term_zero(job) ||
        (term_natural(job) && greedy_repeat(job, sym_number));
}

int term_left_paren(parse_job_t *job)
{
    if (read(job) == '(')
    {
        next(job);
        return true;
    }
    return false;
}

int term_right_paren(parse_job_t *job)
{
    if (read(job) == ')')
    {
        next(job);
        return true;
    }
    return false;
}

int term_natural(parse_job_t *job)
{
    char n = read(job);
    if (n >= '1' && n <= 9)
    {
        next(job);
        return true;
    }
    return false;
}

int term_zero(parse_job_t *job)
{
    if (read(job) == '0')
    {
        next(job);
        return true;
    }
    return false;
}

int parser_process(parser_t *parser, char *str)
{
    parse_job_t job;
    char *up = parser->string;
    job.next_chr = up ? strlen(up) : 0;
    parser->string = str_concat(up, str);
    free(up);

    int ret = one(&job, sym_program);

    return ret;
}
