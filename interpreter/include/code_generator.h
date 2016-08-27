#ifndef CODE_GENERATOR_H_
#define CODE_GENERATOR_H_

#include "token.h"
#include <stdint.h>

int code_generator_generate(token_t* tokens, int token_count,
        uint32_t** instructions, uint32_t* instruction_count,
        uint8_t** data, uint32_t* data_size);

#endif
