#include "../include/parser.h"
#include "../include/error.h"
#include <regex.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

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
    if (a || b)
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
        printf("n: %s", n);
        return n;
    }
    else
    {
        return NULL;
    }

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

typedef struct backtrack_store
{
    int chr;
    int token;
} backtrack_store_t;

typedef struct parse_job
{
    int next_chr;
    int look_ahead;
    parser_t* parser;
    kvec_t(backtrack_store_t) backtrack;
} parse_job_t;

typedef int (*symbol_t)(parse_job_t *);

void store_push(parse_job_t* job)
{
    int current_token = kv_size(job->parser->token_queue) - 1;
    backtrack_store_t s = { job->look_ahead, current_token };
    kv_push(backtrack_store_t ,job->backtrack, s);
}

void store_pop(parse_job_t* job)
{
    int s = kv_size(job->backtrack);
    kv_size(job->backtrack) = s - 1;
}

void store_restore(parse_job_t* job)
{
    backtrack_store_t s = kv_A(job->backtrack, (int)kv_size(job->backtrack) - 1);

    if (s.token >= 0)
    {
        token_t token = kv_A(job->parser->token_queue, s.token);
        job->next_chr = token.index + token.length;
        job->look_ahead = s.chr;
    }
    else
    {
        job->next_chr = 0;
        job->look_ahead = s.chr;
    }
    kv_size(job->parser->token_queue) = s.token + 1;
    store_pop(job);
}

void consume(parse_job_t* job, token_type_t token_type)
{
    int characters = job->look_ahead - job->next_chr;
    printf("consume: [%d,%d] %d: %.*s\n", 
            job->next_chr, characters,
            token_type, 
            characters, job->parser->string + job->next_chr);
    token_t token;
    token.full_string = &job->parser->string;
    token.type = token_type;
    token.length = characters;
    token.index = job->next_chr;
    kv_push(token_t, job->parser->token_queue, token);
    job->next_chr = job->look_ahead;
}

int one(parse_job_t* job, symbol_t symbol)
{
    int chr = job->look_ahead;
    if (job->parser->string[chr] == '\0')
    {
        return 0;
    }
    store_push(job);
    int ret = symbol(job);
    if (ret)
    {
        store_pop(job);
    }
    else
    {
        store_restore(job);
    }
    return ret;
}

inline bool greedy_repeat(parse_job_t* job, symbol_t symbol)
{
    while((one(job, symbol)))
        ;

    return true;
}

inline int repeat_at_least(parse_job_t* job, symbol_t symbol, int least)
{
    store_push(job);
    int i = 0;
    while((one(job, symbol)) != 0)
        i++;
    if (i >= least)
    {
        store_pop(job);
        return i;
    }
    else
    {
        store_restore(job);
        return 0;
    }
}

inline char read(parse_job_t* job)
{
    return job->parser->string[job->look_ahead];
}

inline void next(parse_job_t* job)
{
    job->look_ahead++;
}

inline bool
skip_whitespace(parse_job_t* job)
{
    while(read(job) <= 32 && read(job) != '\0')
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
int sym_binary_operator(parse_job_t* job);
int sym_number(parse_job_t *job);

int tok_push_unit(parse_job_t *job);
int tok_pop_unit(parse_job_t *job);
int tok_integer(parse_job_t *job);

int term_digit(parse_job_t *job);
int term_digit_excluding_zero(parse_job_t *job);
int term_zero(parse_job_t *job);
int term_left_paren(parse_job_t *job);
int term_right_paren(parse_job_t *job);

int sym_program(parse_job_t* job)
{
    store_push(job);
    if (one(job, sym_expression)) 
    {
        store_pop(job);
        return 1;
    }
    else
    {
        store_restore(job);
    }
    return 0;
}

int sym_unit(parse_job_t* job)
{
    store_push(job);
    if ((one(job, tok_push_unit) && 
        skip_whitespace(job) &&
        one(job, sym_expression) && 
        skip_whitespace(job) &&
        one(job, tok_pop_unit)))
    {
        store_pop(job);
        return 1;
    }
    else
    {
        store_restore(job);
    }
    return 0;
}

int sym_expression(parse_job_t* job)
{
    printf("sym_expression(%p)\n", sym_expression);
    // an expression runs for as many operands and 
    // operators as possible. The only requirement is
    // that we MUST end up with one value one the stack
    // after evaluation.
    //
    // Without doing anything else it will not be possible
    // to parse and interpret this live without storing the
    // balance somehow.
    int balance = 0;

    printf("start ");
    store_push(job);
    while(true)
    {
        store_push(job);
        if (one(job, sym_operand))
        {
            balance++;
            printf("balance++ %d\n", balance);
        }
        else if (one(job, sym_binary_operator))
        {
            balance--;
            printf("balance-- %d\n", balance);
        }
        else if (read(job) <= 32 && read(job) > 0)
        {
            next(job);
            printf("balance %d\n", balance);
            skip_whitespace(job);
        }
        else
        {
            store_restore(job);
            printf("end balance %d\n", balance);
            if (balance != 1)
            {
                store_restore(job);
                return 0;
            }
            else
            {
                store_pop(job);
                return 1;
            }
        }

        store_pop(job);
    }
}

int sym_operand(parse_job_t* job)
{
    return one(job, sym_unit) || one(job, tok_integer);
}

int sym_binary_operator(parse_job_t *job)
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
            
            next(job);
            consume(job, t);
            return true;

        default:
            return false;
    }
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
    int ret = try_tokenize(job, sym_number, TOK_INTEGER);
    return ret;
}

int sym_number(parse_job_t *job)
{
    return term_zero(job) ||
        (term_digit_excluding_zero(job) && greedy_repeat(job, term_digit));
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
        return 1;
    }
    return 0;
}

int term_digit_excluding_zero(parse_job_t *job)
{
    char n = read(job);
    if (n >= '1' && n <= '9')
    {
        next(job);
        return 1;
    }
    return 0;
}

int term_digit(parse_job_t *job)
{
    char n = read(job);
    if (n >= '0' && n <= '9')
    {
        next(job);
        return 1;
    }
    return 0;
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

void print_tokens(token_t* tokens, int len)
{
    token_t* end = tokens + len;
    printf("%d: ", len);
    while(tokens != end)
    {
        char* str = (char *)malloc(tokens->length);
        strncpy(str, (char *)*tokens->full_string + tokens->index, tokens->length);
        int p = tokens->type != TOK_NONE;
        if (p)
        {
            printf("[%d,%d]%.*s", tokens->index, tokens->length, tokens->length, str);
        }

        free(str);
        tokens++;
        if (tokens != end)
        {
            if (p)
            {
                printf(", ");
            }
        }
    }
    printf("\n");
}

int parser_process(parser_t *parser, char *str)
{
    parse_job_t job = {};
    char *up = parser->string;
    parser->string = str_concat(up, str);
    int tokens = kv_size(parser->token_queue);
    job.next_chr = 0;
    if (tokens != 0)
    {
        token_t last = kv_A(parser->token_queue, tokens - 1);
        job.next_chr = last.index + last.length;
    }
    job.look_ahead = job.next_chr;
    printf("next_chr: %d", job.next_chr);
    job.parser = parser;
    kv_init(job.backtrack);
    free(up);
    up = NULL;

    int prev = kv_size(parser->token_queue);
    int ret = one(&job, sym_program);
    int now = kv_size(parser->token_queue);
    int len = now - prev;
    kv_destroy(job.backtrack);

    printf("prev %d", prev);
    print_tokens(&kv_A(parser->token_queue, prev), len);

    return ret;
}

