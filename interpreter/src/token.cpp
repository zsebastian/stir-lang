#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>
#include "../include/token.h"
#include "../include/error.h"

int ensure_buffer_size(char **buffer, int* size_ptr, int new_size)
{
    int min_size = 20;
    float size_alloc_factor = 1.5f;
    int size = *size_ptr;

    if (*buffer == NULL)
    {
        *buffer = (char *)malloc(min_size);
        if (*buffer == NULL)
        {
            return ERR_MALLOC_FAILED;
        }


        *size_ptr = min_size;
        return ERR_SUCCESS;
    }
    if (new_size > size)
    {
        int alloc_size = new_size * size_alloc_factor;
        char *ptr = (char *)malloc(alloc_size);
        if (ptr == NULL)
        {
            *size_ptr = 0;
            return ERR_MALLOC_FAILED;
        }
        memcpy(ptr, *buffer, size);
        free(*buffer); 
        *buffer = ptr;
        *size_ptr = alloc_size;
    }
    return ERR_SUCCESS;
}

int ensure_token_array_size(token_t **queue, int* size_ptr, int new_size)
{
    int min_size = 20;
    float size_alloc_factor = 1.5f;
    int size = *size_ptr;

    if (*queue == NULL)
    {
        *queue = (token_t*)malloc(min_size * sizeof(token_t));
        if (*queue == NULL)
        {
            return ERR_MALLOC_FAILED;
        }

        *size_ptr = min_size;
        return ERR_SUCCESS;
    }
    if (new_size > size)
    {
        int alloc_size = new_size * size_alloc_factor * sizeof(token_t);
        token_t *ptr = (token_t*)malloc(alloc_size);
        if (ptr == NULL)
        {
            *size_ptr = 0;
            return ERR_MALLOC_FAILED;
        }
        memcpy(ptr, *queue, size);
        free(*queue); 
        *queue = ptr;
        *size_ptr = alloc_size;
    }
    return ERR_SUCCESS;
}

int push_token(token_array_t* token_queue, token_t token)
{
    int ensure = ensure_token_array_size(&token_queue->buffer, 
            &token_queue->buffer_reserve, 
            token_queue->buffer_len + 1);
    if (ensure != ERR_SUCCESS)
    {
        return ensure;    
    }

    token_queue->buffer[token_queue->buffer_len] = token;
    token_queue->buffer_len++;
    return ERR_SUCCESS;
}

int eval_token_queue(token_state_t* state)
{
    for(; state->current_token_queue_position < state->token_queue.buffer_len;
            ++state->current_token_queue_position)
    {
        printf("%d", state->current_token_queue_position);
        int top = state->eval_stack.buffer_len;
        int next = state->current_token_queue_position;

        if (state->token_queue.buffer[next].token_type == WHITESPACE)
        {
            continue;
        }

        int ensure = ensure_token_array_size(&state->eval_stack.buffer, 
                &state->eval_stack.buffer_reserve, 
                state->eval_stack.buffer_len + 1);

        if (ensure != ERR_SUCCESS)
        {
            return ensure;    
        }

        state->eval_stack.buffer[top] = state->token_queue.buffer[next];

        state->eval_stack.buffer_len++;
    }

    printf("Current stack: ");

    for(int i = 0; i < state->eval_stack.buffer_len; i++)
    {
        token_t token = state->eval_stack.buffer[i];
        int len = token.build_buffer_end - token.build_buffer_begin;
        char str[len + 1];
        for(int j = 0; j < len; ++j)
        {
            str[j] = state->build_buffer[token.build_buffer_begin + j];
        }
        str[len] = '\0';
        printf("[%d - %d: %s] ", token.build_buffer_begin, token.build_buffer_end, str);
    }

    return ERR_SUCCESS;
}

int process_buffer(token_state_t* state, int fromIndex)
{
    regmatch_t match;
    char* buffer_begin = state->build_buffer + fromIndex;
    char* buffer_end = state->build_buffer + state->build_buffer_len - 1;
    printf("processing buffer with len %ld\n", buffer_end - buffer_begin);
    printf("complete buffer: %s", state->build_buffer);
    while(buffer_begin != buffer_end)
    {
        token_type_t token_type;
        if (!regexec(&state->int_regex, buffer_begin, 1, &match, 0))
        {
            printf("int");
            token_type = INT;
        }
        else if (!regexec(&state->whitespace_regex, buffer_begin, 1, &match, 0))
        {
            printf("whitespace");
            token_type = WHITESPACE;
        }
        else if (!regexec(&state->operator_regex, buffer_begin, 1, &match, 0))
        {
            printf("operator");
            token_type = OPERATOR;
        }
        else
        {
            printf("Unexpected token at %ld: \"%s\"", buffer_begin - state->build_buffer, buffer_begin);
            return ERR_UNEXPECTED_TOKEN;
        }
        
        token_t token;
        token.build_buffer_begin = buffer_begin - state->build_buffer;
        buffer_begin += match.rm_eo;
        token.build_buffer_end = buffer_begin - state->build_buffer;
        token.token_type = token_type;

        printf(" Consumed: %d, begin: %d, end: %d, left: %d\n", 
                match.rm_eo, 
                token.build_buffer_begin,
                token.build_buffer_end, 
                (int)(buffer_begin - buffer_end));

        int err = push_token(&state->token_queue, token);
        if (err != ERR_SUCCESS)
        {
            return err;
        }
    } 

    int err = eval_token_queue(state);
    if (err != ERR_SUCCESS)
    {
        return err;
    }

    return ERR_SUCCESS;
}

int token_init(token_state_t **state)
{
    *state = (token_state_t*)malloc(sizeof(token_state_t));
    if (*state == NULL)
    {
        return ERR_MALLOC_FAILED;
    }
    (*state)->build_buffer_reserve = 0;
    (*state)->build_buffer_len = 0;

    (*state)->token_queue.buffer = NULL;
    (*state)->token_queue.buffer_len = 0;
    (*state)->token_queue.buffer_reserve = 0;

    {
        int ensure = ensure_token_array_size(&(*state)->token_queue.buffer, 
            &((*state)->token_queue).buffer_reserve, 
            0);
        if(ensure != ERR_SUCCESS)
        {
            return ensure;
        }
    }

    (*state)->eval_stack.buffer = NULL;
    (*state)->eval_stack.buffer_len = 0;
    (*state)->eval_stack.buffer_reserve = 0;
    (*state)->current_token_queue_position = 0;
    {
        int ensure = ensure_token_array_size(&(*state)->eval_stack.buffer, 
            &((*state)->eval_stack).buffer_reserve, 
            0);
        if(ensure != ERR_SUCCESS)
        {
            return ensure;
        }
    }

    const char* int_regex_str = "^[0-9][0-9]*";
    const char* operator_regex_str = "^[\\+\\-\\/\\%]\\|[*]";
    const char* whitespace_regex_str = "^[ \t\n\r\f\v][ \t\n\r\f\v]*";
    int reti = regcomp(&(*state)->int_regex, int_regex_str, 0);
    int reto = regcomp(&(*state)->operator_regex, operator_regex_str, 0);
    int retw = regcomp(&(*state)->whitespace_regex, whitespace_regex_str, 0);

    if (reti)
    {
        printf("failed to compile int regex: %d.\n", reti);
    }
    if (reto)
    {
        printf("failed to compile operator regex: %d.\n", reto);
    }
    if (retw)
    {
        printf("failed to compile whitespace regex: %d.\n", retw);
    }

    int ensure = ensure_buffer_size(&(*state)->build_buffer, 
            &(*state)->build_buffer_reserve, 1);
    if(ensure != ERR_SUCCESS)
    {
        return ensure;
    }
    (*state)->build_buffer[0] = '\0';
    (*state)->build_buffer_len = 1;

    return ERR_SUCCESS;
}

int token_process(token_state_t *current_state, char* data)
{
    int len = strlen(data);
    printf("precessing %s (%d)", data, len);
    int new_len = current_state->build_buffer_len + len; 

    int ensure = ensure_buffer_size(&current_state->build_buffer, 
            &current_state->build_buffer_reserve, new_len + 1);
    if(ensure != ERR_SUCCESS)
    {
        return ERR_MALLOC_FAILED;
    }

    strcpy(current_state->build_buffer + current_state->build_buffer_len - 1, data);
    current_state->build_buffer_len = new_len;

    current_state->build_buffer[current_state->build_buffer_len - 1] = '\0';

    printf("\n[");
    int from = new_len - len - 1;
    for(int i = 0; i < current_state->build_buffer_len; ++i)
    {
        if (i == from)
        {
            printf("_");
        }
        printf("%d", (int) current_state->build_buffer[i]);
        if (i == from)
        {
            printf("_");
        }
        printf(" ");
    }
    printf("]\n");

    int ret = process_buffer(current_state, new_len - len - 1);
    
    return ret;
}

int token_destroy(token_state_t **state)
{
    free(state);

    return ERR_SUCCESS;
}
