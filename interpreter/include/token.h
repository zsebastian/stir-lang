#ifndef TOKEN_H
#define TOKEN_H

#include <regex.h>

typedef enum { INT, OPERATOR, WHITESPACE} token_type_t;

typedef struct
{
    token_type_t token_type;
    int build_buffer_begin;
    int build_buffer_end;
} token_t;

typedef struct
{
    token_t *buffer;
    int buffer_reserve;
    int buffer_len;
} token_array_t;

typedef struct 
{
    token_array_t token_queue;
    char* build_buffer;
    int build_buffer_reserve;
    int build_buffer_len;

    token_array_t eval_stack;
    int current_token_queue_position; //which position are we evaluating

    regex_t int_regex; 
    regex_t operator_regex; 
    regex_t whitespace_regex; 
    regex_t unit_regex; 
} token_state_t;

int token_init(token_state_t **state);

int token_process(token_state_t *current_state, char* string);

int token_destroy(token_state_t **state);

#endif
