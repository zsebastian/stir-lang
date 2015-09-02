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

int ensure_token_queue_size(token_t **queue, int* size_ptr, int new_size)
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

int push_token(token_queue_t* token_queue, token_t token)
{
    int ensure = ensure_token_queue_size(&token_queue->buffer, 
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

int process_buffer(token_state_t* state)
{
    regmatch_t match;
    char* buffer_begin = state->build_buffer;
    char* buffer_end = buffer_begin + state->build_buffer_len - 1;
    printf("processing buffer with len %ld\n", buffer_end - buffer_begin);
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
            printf("Unexpected token at %ld", buffer_end - buffer_begin);
            return ERR_UNEXPECTED_TOKEN;
        }
        
        token_t token;
        token.begin = buffer_begin;
        buffer_begin += match.rm_eo;
        token.end = buffer_begin;
        token.token_type = token_type;

        printf("Consumed: %d, begin: %ld end: %ld\n", 
                match.rm_eo, 
                token.begin - state->build_buffer,
                token.end - state->build_buffer);

        push_token(&state->token_queue, token);
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
        int ensure = ensure_token_queue_size(&(*state)->token_queue.buffer, 
            &((*state)->token_queue).buffer_reserve, 
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

    int ret = process_buffer(current_state);
    
    current_state->build_buffer[0] = '\0';
    current_state->build_buffer_len = 1;
    return ret;
}

int token_destroy(token_state_t **state)
{
    free(state);

    return ERR_SUCCESS;
}
