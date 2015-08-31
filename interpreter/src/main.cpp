#include <stdio.h>
#include <stdlib.h>
#include "../include/token.h"

char* read_stdin(void);

int main(int argc, char **argv)
{
    token_state_t* token_state;
    token_init(&token_state);
    printf("\n>"); 
    char* line;
    while ((line = read_stdin()))
    {
        if (line != NULL)
        {
            token_process(token_state, line);
        }
        printf("\n>"); 
    }
    token_destroy(&token_state);
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
