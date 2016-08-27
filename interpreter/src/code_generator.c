#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/code_generator.h"
#include "../include/error.h"
#include "../modules/klib/kvec.h"
#include "../include/bytecode.h"

typedef kvec_t(uint32_t) instructions_t;
typedef kvec_t(uint8_t) data_t;

void consume_token(instructions_t *ivec, 
        data_t* program_data, token_t token);

int code_generator_generate(token_t* tokens, int token_count,
        uint32_t** instructions, uint32_t* instruction_count,
        uint8_t** data, uint32_t* data_size)
{
    instructions_t ivec;
    kv_init(ivec);
    data_t datavec;
    kv_init(datavec);

    token_t* end = tokens + token_count;
    while(tokens != end)
    {
        consume_token(&ivec, &datavec, *tokens);
        tokens++;
    }
    int ibs = kv_size(ivec) * sizeof(uint32_t);
    printf("ibs %d\n", ibs);
    *instructions =  (uint32_t *)malloc(ibs);
    printf("ptr %p\n", *instructions);
    memcpy(*instructions, &kv_A(ivec, 0), ibs);
    *instruction_count = kv_size(ivec);

    int dbs = kv_size(datavec) * sizeof(uint8_t);
    printf("dbs %d\n", dbs);
    *data = (uint8_t *)malloc(dbs);
    memcpy(*data, &kv_A(datavec, 0), dbs);
    *data_size = kv_size(datavec);

    kv_destroy(ivec);
    kv_destroy(datavec);

    return ERR_SUCCESS;
}

int push_instruction(instructions_t *instructions, uint32_t bytecode)
{
    kv_push(uint32_t, *instructions, bytecode);
    return kv_size(*instructions) - 1;
}

int push_data(data_t* datavec, void* data, int count)
{
    int start = kv_size(*datavec);
    uint8_t* begin = (uint8_t *)data;
    uint8_t* end = begin + count;
    while(begin != end)
    {
        kv_push(uint8_t, *datavec, *begin);
    }
    return start;
}

long _to_long(char* str, int i, int len)
{
    if (len > 64)
    {
        printf("too long: cannot convert to long%d:%d", i, len);
        return 0;
    }
    char s[65];
    strncpy(s, str + i, len);
    s[64] = '\0';
    return strtol(s, NULL, 10);
}

long to_long(token_t token)
{
    return _to_long(*token.full_string, token.index, token.length);
}

void consume_token(instructions_t *ivec,
        data_t* program_data, token_t token)
{
    switch(token.type)
    {
        case (TOK_INTEGER):
        {
            push_instruction(ivec, bytecode_iconst(to_long(token)));
        } break;
        case (TOK_ADDITION):
        {
            push_instruction(ivec, bytecode_pop(0));
            push_instruction(ivec, bytecode_pop(1));
            push_instruction(ivec, bytecode_iadd(0, 1 ,2));
            push_instruction(ivec, bytecode_push(2));
        } break;
        case (TOK_SUBTRACTION):
        {
            push_instruction(ivec, bytecode_pop(1));
            push_instruction(ivec, bytecode_pop(0));
            push_instruction(ivec, bytecode_isub(0, 1 ,2));
            push_instruction(ivec, bytecode_push(2));
        } break;
        default:
            break;
    }
}
