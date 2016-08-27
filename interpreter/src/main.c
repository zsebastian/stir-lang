#include <stdio.h>
#include <stdlib.h>
#include "../include/parser.h"
#include "../include/bytecode.h"
#include "../include/cpu.h"

char* read_stdin(void);

int main(int argc, char **argv)
{
    uint32_t memory[32] = {0};

    uint32_t instructions[] = 
    {
        bytecode_iconst(32),
        bytecode_iconst(10),
        bytecode_pop(0),
        bytecode_pop(1),
        bytecode_iadd(0, 1, 2),
        bytecode_store(2, 0),
        bytecode_push(2),
        bytecode_halt(0)
    };
    int ret = cpu_execute(instructions, sizeof(instructions) / sizeof(instructions[0]),
            memory, sizeof(memory) / sizeof(memory[0]), 0);

    printf("[%d]\n", ret);

    int i = 0;
    for(; i < sizeof(memory) / sizeof(memory[0]); i++)
    {
        if ((i % 8) == 0)
        {
            printf("\n");
        }
        printf("%02x ", (memory[i] & 0xFF000000) >> 24);
        printf("%02x ", (memory[i] & 0x00FF0000) >> 16);
        printf("%02x ", (memory[i] & 0x0000FF00) >> 8);
        printf("%02x ", memory[i] & 0x000000FF);
    }
    return 0;
    parser_t parser_val;
    parser_t *parser = &parser_val;
    parser_init(parser);
    printf("\n>"); 
    char* line;
    while ((line = read_stdin()))
    {
        if (line != NULL)
        {
            printf("%s", line);
            parser_process(parser, line);
        }
        printf("\n>"); 
    }
    parser_free(parser);
    return 0;
}

char* read_stdin(void) {
    char* line = (char*)malloc(100), * linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if(line == NULL)
        return NULL;

    for(;;) {
        c = fgetc(stdin);
        if(c == EOF)
            break;

        if(--len == 0) {
            len = lenmax;
            char* linen = (char*)realloc(linep, lenmax *= 2);

            if(linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if((*line++ = c) == '\n')
            break;
    }
    *line = '\0';
    return linep;
}
