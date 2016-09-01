#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/code_generator.h"
#include "../include/error.h"
#include "../modules/klib/kvec.h"
#include "../include/bytecode.h"
#include <assert.h>
#include "../include/str_to_u32.h"

#pragma GCC diagnostic ignored "-Wunused-value"

typedef enum data_type
{
    TYPE_INT32,
    TYPE_LAMBDA
} data_type_t;

int bytesize(data_type_t type)
{
    switch(type)
    {
        case TYPE_INT32:
            return 4;
        case TYPE_LAMBDA:
            return 2;
        default:
            assert(0);
    }
}

int is_signed(data_type_t type)
{
    switch(type)
    {
        case TYPE_INT32:
            return 1;
        case TYPE_LAMBDA:
            return 2;
        default:
            return 0;
    }
}

int is_unsigned(data_type_t type)
{
    switch(type)
    {
        case TYPE_INT32:
            return 0;
        default:
            return 1;
    }
}

data_type_t resolve_type(data_type_t t0, data_type_t t1)
{
    if (t0 == t1)
    {
        return t0;
    }
    printf("type resolution not implemented for types %d-%d\n", t0, t1);
    return t0;
}

typedef kvec_t(uint32_t) instructions_t;
typedef kvec_t(uint8_t) data_t;
typedef kvec_t(data_type_t) dtvec_t;
typedef kvec_t(str_to_u32_t *) id_stack_t;

typedef struct state
{
    instructions_t *instructions;
    data_t* program_data;
    dtvec_t* data_type_stack;
    id_stack_t* id_address_stack;
} state_t;

inline void push_instruction_address_stack(state_t* state)
{
    kv_push(str_to_u32_t*, *state->id_address_stack, str_to_u32_create());
}

inline void pop_instruction_address_stack(state_t* state)
{
    str_to_u32_t* old = kv_A(*state->id_address_stack, kv_size(*state->id_address_stack) -1);
    str_to_u32_destroy(old);
    kv_pop(*state->id_address_stack);
}
void consume_token(state_t* state, token_t** current_token);

int code_generator_generate(token_t* tokens, int token_count,
        uint32_t** instructions, uint32_t* instruction_count,
        uint8_t** data, uint32_t* data_size)
{
    instructions_t ivec;
    kv_init(ivec);

    data_t datavec;
    kv_init(datavec);

    dtvec_t dtvec;
    kv_init(dtvec);

    id_stack_t iasvec;
    kv_init(iasvec);

    state_t state =
    {
        &ivec, &datavec, &dtvec, &iasvec
    };

    token_t* end = tokens + token_count;

    push_instruction_address_stack(&state);
    while(tokens != end)
    {
        consume_token(&state, &tokens);
        tokens++;
    }
    printf("done\n");
    pop_instruction_address_stack(&state);
    int ibs = kv_size(ivec) * sizeof(uint32_t);
    *instructions =  (uint32_t *)malloc(ibs);
    memcpy(*instructions, &kv_A(ivec, 0), ibs);
    *instruction_count = kv_size(ivec);

    int dbs = kv_size(datavec) * sizeof(uint8_t);
    *data = (uint8_t *)malloc(dbs);
    memcpy(*data, &kv_A(datavec, 0), dbs);
    *data_size = kv_size(datavec);

    kv_destroy(ivec);
    kv_destroy(datavec);
    kv_destroy(dtvec);

    printf("Code generated\n");

    printf("\n");
    printf("%d instructions: %p\n", *instruction_count, *instructions);
    uint32_t *ip = *instructions;
    uint32_t *i_end = (*instructions + *instruction_count);

    while(ip != i_end)
    {
        uint32_t value;
        uint8_t code, r0, r1, r2;
        uint16_t addr;
        _bytecode_read(*ip, &code, &value, &addr,
                &r0, &r1, &r2);
        printf("[%d] %8x\n", (int)(ip - *instructions), *ip);
        printf("  c: %d r0: %d r1: %d r2: %d addr: %d value: %d\n", 
                code, r0, r1, r2, addr, value);
        ip++;
    }

    return ERR_SUCCESS;
}

inline uint32_t push_instruction(state_t *state, uint32_t bytecode)
{
    instructions_t* ivec = state->instructions;
    kv_push(uint32_t, *ivec, bytecode);
    return kv_size(*ivec) - 1;
}

inline void set_instruction(state_t *state, uint32_t bytecode, uint32_t address)
{
    instructions_t* ivec = state->instructions;
    kv_A(*ivec, address) = bytecode;
}

inline int get_instruction_address(state_t *state)
{
    instructions_t* ivec = state->instructions;
    return kv_size(*ivec) - 1;
}

inline int push_data(state_t *state, void *data, int count)
{
    data_t* datavec = state->program_data;
    int start = kv_size(*datavec);
    uint8_t* begin = (uint8_t *)data;
    uint8_t* end = begin + count;
    while(begin != end)
    {
        kv_push(uint8_t, *datavec, *begin);
    }
    return start;
}

inline int push_type(state_t *state, data_type_t type)
{
    dtvec_t* dtvec = state->data_type_stack;
    kv_push(data_type_t, *dtvec, type);
    return kv_size(*dtvec) - 1;
}

inline data_type_t top_type(state_t *state, int offset)
{
    dtvec_t* dtvec = state->data_type_stack;
    return kv_A(*dtvec, (kv_size(*dtvec) - 1 - offset));
}

inline data_type_t pop_type(state_t *state)
{
    dtvec_t* dtvec = state->data_type_stack;
    data_type_t top = top_type(state, 0);
    kv_pop(*dtvec);
    return top;
}

inline long _to_long(char* str, int i, int len)
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

inline long to_long(token_t token)
{
    return _to_long(*token.full_string, token.index, token.length);
}

inline double _to_double(char* str, int i, int len)
{
    if (len > 64)
    {
        printf("too long: cannot convert to double%d:%d", i, len);
        return 0;
    }
    char s[65];
    strncpy(s, str + i, len);
    s[64] = '\0';
    return atof(s);
}

inline double to_double(token_t token)
{
    return _to_double(*token.full_string, token.index, token.length);
}

inline uint32_t get_instruction_address_of_identifier(state_t* state, token_t token)
{
    char *str = *token.full_string + token.index;
    char *tmp = malloc(token.length + 1);
    strncpy(tmp, str, token.length);
    tmp[token.length] = '\0';
    int i = kv_size(*state->id_address_stack) - 1;
    uint32_t ret = 0;

    while(i >= 0 && 
            !str_to_u32_get(kv_A(*state->id_address_stack, i), tmp, &ret))
    {
        printf("  try %s -> %d\n", tmp, ret);
        i--;
    }
    printf("get %s -> %d\n", tmp, ret);
    if (i < 0)
    {
        // just make sure we actually return 0
        return 0;
    }
    return ret;

}

inline void set_instruction_address_of_identifier(state_t *state, token_t token, uint32_t address)
{
    char *str = *token.full_string + token.index;
    char *tmp = malloc(token.length + 1);
    strncpy(tmp, str, token.length);
    tmp[token.length] = '\0';
    printf("set %s -> %d\n", tmp, address);
    str_to_u32_t* top = kv_A(*state->id_address_stack, kv_size(*state->id_address_stack) - 1);
    str_to_u32_set(top, tmp, address);
}

void print_token(token_t token)
{
    char* str = (char *)malloc(token.length);
    strncpy(str, (char *)*token.full_string + token.index, token.length);
    int p = token.type != TOK_NONE;
    if (p)
    {
        printf("[%d,%d]%.*s\n", token.index, token.length, token.length, str);
    }

    free(str);
}

void consume_token(state_t* state, token_t **current_token)
{
    token_t token = **current_token;

    print_token(token);
    switch(token.type)
    {
        case (TOK_INTEGER):
        {
            push_instruction(state, bytecode_iconst(to_long(token)));
            push_type(state, TYPE_INT32);
        } break;
        case (TOK_ADDITION):
        {
            printf("a\n");
            // The type is whatever the type of the type resolution
            data_type_t dt0 = pop_type(state);
            data_type_t dt1 = pop_type(state);
            data_type_t dt2 = resolve_type(dt0, dt1);
            push_instruction(state, bytecode_pop(REG_I32_A, bytesize(dt0)));
            push_instruction(state, bytecode_pop(REG_I32_B, bytesize(dt1)));
            printf("b\n");
            switch(dt2)
            {
                case(TYPE_INT32):
                    push_instruction(state, bytecode_iadd(REG_I32_A, REG_I32_B, REG_I32_C));
                    break;
                default:
                    printf("Can only divide to integers");
                    return;
            }
            push_instruction(state, bytecode_push(REG_I32_C, bytesize(dt2)));

            push_type(state, dt2);
            printf("c\n");
        } break;
        case (TOK_SUBTRACTION):
        {
            data_type_t dt0 = pop_type(state);
            data_type_t dt1 = pop_type(state);
            data_type_t dt2 = resolve_type(dt0, dt1);
            push_instruction(state, bytecode_pop(REG_I32_A, bytesize(dt0)));
            push_instruction(state, bytecode_pop(REG_I32_B, bytesize(dt1)));
            switch(dt2)
            {
                case(TYPE_INT32):
                    push_instruction(state, bytecode_isub(REG_I32_A, REG_I32_B, REG_I32_C));
                    break;
                default:
                    printf("Can only divide to integers");
                    return;
            }
            push_instruction(state, bytecode_push(REG_I32_C, bytesize(dt2)));
            
            push_type(state, dt2);
        } break;
        case (TOK_MULTIPLICATION):
        {
            data_type_t dt0 = pop_type(state);
            data_type_t dt1 = pop_type(state);
            data_type_t dt2 = resolve_type(dt0, dt1);
            push_instruction(state, bytecode_pop(REG_I32_A, bytesize(dt0)));
            push_instruction(state, bytecode_pop(REG_I32_B, bytesize(dt1)));
            switch(dt2)
            {
                case(TYPE_INT32):
                    push_instruction(state, bytecode_imult(REG_I32_A, REG_I32_B, REG_I32_C));
                    break;
                default:
                    printf("Can only divide to integers");
                    return;
            }
            push_instruction(state, bytecode_push(REG_I32_C, bytesize(dt2)));
            
            push_type(state, dt2);
        } break;
        case (TOK_DIVISION):
        {
            data_type_t dt0 = pop_type(state);
            data_type_t dt1 = pop_type(state);
            data_type_t dt2 = resolve_type(dt0, dt1);
            push_instruction(state, bytecode_pop(REG_I32_A, bytesize(dt0)));
            push_instruction(state, bytecode_pop(REG_I32_B, bytesize(dt1)));
            switch(dt2)
            {
                case(TYPE_INT32):
                    push_instruction(state, bytecode_idiv(REG_I32_A, REG_I32_B, REG_I32_C));
                    break;
                default:
                    printf("Can only divide to integers");
                    return;
            }
            push_instruction(state, bytecode_push(REG_I32_C, bytesize(dt2)));
            
            push_type(state, dt2);
        } break;
        case (TOK_PUSH_LAMBDA_UNIT):
        {
            int b = 1;
            push_instruction_address_stack(state);
            token_t *t = *current_token;
            while(b != 0)
            {
                t++;
                switch(t->type)
                {
                    case(TOK_PUSH_LAMBDA_UNIT):
                        b++;
                        break;
                    case(TOK_POP_LAMBDA_UNIT):
                        b--;
                        break;
                    default:
                        break;
                }
            }
            print_token(*t);
            push_type(state, TYPE_LAMBDA);
            // store our pushed address so the same pop unit can set it 
            // so that it can jump out of itself.
            set_instruction_address_of_identifier(state, *t, push_instruction(state, 0));
        } break;
        case (TOK_POP_LAMBDA_UNIT):
        {
            data_type_t dt = pop_type(state);
            push_instruction(state, bytecode_ret());
            push_type(state, dt);

            uint32_t address = get_instruction_address_of_identifier(state, token);
            uint32_t next_address = get_instruction_address(state);
            set_instruction(state, bytecode_iconst(next_address), address);
            pop_instruction_address_stack(state);
            // The type is whatever the type of the type resolution
            // of the unity.
        } break;
        case (TOK_IDENTIFIER):
        {
            // Have I forgotten what units are? This doesn't work
            pop_type(state);
            uint32_t address = get_instruction_address_of_identifier(state, token);
            push_instruction(state, bytecode_iconst(address));
            push_instruction(state, bytecode_popip(REG_I32_A));
            push_instruction(state, bytecode_jmp(REG_I32_A));
        } break;
        case (TOK_PULL_OPERATOR):
        {
            set_instruction_address_of_identifier(state, 
                *(*current_token + 1), 
                get_instruction_address(state) - 1);
            (*current_token) = *current_token + 1;
        } break;
        case (TOK_NONE):
            break;
        default:
            printf("Unknown token\n");
            break;
    }
}
