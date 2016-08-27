#ifndef PARSER_H_
#define PARSER_H_

#include "../modules/klib/kvec.h"
#include "../include/token.h"

typedef struct parser
{
    kvec_t(token_t) token_queue;
    
    /* All tokens are stored here */
    char *string; 
} parser_t;

int parser_free(parser_t* parser);

int parser_init(parser_t* parser);

int parser_process(parser_t* parser, char* str);

#endif
