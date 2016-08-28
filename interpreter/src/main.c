#include <stdio.h>
#include <stdlib.h>
#include "../include/parser.h"
#include "../include/bytecode.h"
#include "../include/cpu.h"
#include "../modules/parg/parg.h"
#include "../include/code_generator.h"

char* read_stdin(void);

int main(int argc, char **argv)
{
    struct parg_state ps;
    int c;
    int interactive = 0;
    char *execute = NULL;
    int t = 0;
    int p = 0;

    parg_init(&ps);
    while((c = parg_getopt(&ps, argc, argv, "pthie:")) != -1)
    {
        switch(c)
        {
            case 1:
                printf("nonoption '%s'\n", ps.optarg);
                break;
            case 'i':
                interactive++;
                break;
            case 'e':
                execute = (char *)ps.optarg;
                break;
            case 't':
                t++;
                break; 
            case 'p':
                p++;
                break; 
            case 'h':
                printf("usage: stir [-i input]\n"); 
                return EXIT_SUCCESS;
            default:
                printf("unknown option: %c", (char)c);
        }
    }

    if (t)
    {
        uint8_t memory[32];

        uint32_t instructions[] = 
        {
            bytecode_iconst(32),
            bytecode_iconst(10),
            bytecode_pop(0, 4),
            bytecode_pop(1, 4),
            bytecode_iadd(0, 1, 2),
            bytecode_store(2, 0),
            bytecode_push(2, 4),
            bytecode_halt(0)
        };
        int ret = cpu_execute(instructions, sizeof(instructions) / sizeof(instructions[0]),
                memory, sizeof(memory) / sizeof(memory[0]), 0);

        printf("[%d]\n", ret);

        int i = 0;
        for(; i < sizeof(memory) / sizeof(memory[0]); i++)
        {
            if ((i % 16) == 0)
            {
                printf("\n");
            }
            printf("%02x ", memory[i]);
        }
    }
    printf("\n----------\n");
    parser_t parser_val;
    parser_t *parser = &parser_val;
    parser_init(parser);
    if (execute != NULL)
    {
        uint32_t* instructions;
        uint8_t* data;
        uint32_t i_count;
        uint32_t d_count;

        token_t* tokens;
        int t_count;
        parser_process(parser, execute, &tokens, &t_count);

        code_generator_generate(tokens, t_count, 
                &instructions, &i_count,
                &data, &d_count);

        printf("ptr %p", instructions);

        // TODO(sebe): copy over the data to the memory
        // and structure the program's memory sanely
        uint8_t memory[32];
        d_count = 32;

        if (!p)
        {
            int ret = cpu_execute(instructions, i_count,
                    memory, d_count, 0);

            printf("[%d]\n", ret);

            int i = 0;
            for(; i < d_count; i++)
            {
                if ((i % 16) == 0)
                {
                    printf("\n");
                }
                printf("%02x ", memory[i]);
            }
        }
    }

    if (interactive)
    {
        printf("\n>"); 
        char* line;
        while ((line = read_stdin()))
        {
            if (line != NULL)
            {
                token_t* tokens;
                int t_count;
                printf("%s", line);
                parser_process(parser, line, &tokens, &t_count);
            }
            printf("\n>"); 
        }
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
