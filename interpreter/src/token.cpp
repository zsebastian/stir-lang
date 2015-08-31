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

int pop_back_buffer(char* buffer, int len, int pops)
{
    for (int i = 0; i < len; ++i)
    {
        buffer[i] = buffer[i + pops];
    }
    return ERR_SUCCESS;
}

int process_buffer(token_state_t* state)
{
    regmatch_t match;
    int consumed = -1;
    while(consumed != 0)
    {
        consumed = 0;
        int reti = regexec(&state->int_regex, state->build_buffer, 1, &match, 0) ;
        int retw = regexec(&state->whitespace_regex, state->build_buffer, 1, &match, 0) ;
        if (!reti)
        {
            consumed = match.rm_eo;
        }
        if (!retw)
        {
            consumed = match.rm_eo;
        }
        printf("Consumed: %d\n", consumed);
        pop_back_buffer(state->build_buffer, state->build_buffer_len, consumed);
        printf("After: %s\n", state->build_buffer);
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

    const char* int_regex_str = "^[0-9][0-9]*";
    const char* operator_regex_str = "^(+|-|*|/)";
    const char* whitespace_regex_str = "^[ \t\n\r\f\v][ \t\n\r\f\v]*";
    const char* unit_regex_str = "^\\(|\\)s";
    int reti = regcomp(&(*state)->int_regex, int_regex_str, 0);
    int reto = regcomp(&(*state)->operator_regex, operator_regex_str, 0);
    int retw = regcomp(&(*state)->whitespace_regex, whitespace_regex_str, 0);
    int retu = regcomp(&(*state)->unit_regex, unit_regex_str, 0);

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
    if (retu)
    {
        printf("failed to compile unit regex: %d.\n", retu);
    }

    int ensure = ensure_buffer_size(&(*state)->build_buffer, 
            &(*state)->build_buffer_reserve, 1);
    if(ensure != ERR_SUCCESS)
    {
        return ERR_MALLOC_FAILED;
    }
    (*state)->build_buffer[0] = '\n';

    return ERR_SUCCESS;
}

int token_process(token_state_t *current_state, char* data)
{
    int len = strlen(data);
    int new_len = current_state->build_buffer_len + len; 

    int ensure = ensure_buffer_size(&current_state->build_buffer, 
            &current_state->build_buffer_reserve, new_len + 1);
    if(ensure != ERR_SUCCESS)
    {
        return ERR_MALLOC_FAILED;
    }

    strcpy(current_state->build_buffer + current_state->build_buffer_len, data);
    current_state->build_buffer_len = new_len;

    return process_buffer(current_state);
}

int token_destroy(token_state_t **state)
{
    free(state);

    return ERR_SUCCESS;
}
