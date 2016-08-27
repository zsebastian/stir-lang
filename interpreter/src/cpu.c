#include "../include/cpu.h"
#include "../include/bytecode.h"
#include <stdint.h>
#include <stdio.h>


int cpu_execute(uint32_t* instructions, int instructions_size,
        uint32_t* memory, int memory_size,
        int entry_point)
{
    uint32_t *ip = instructions + entry_point;
    uint32_t *end = instructions + instructions_size;
    uint32_t *stack = memory + memory_size;
    uint8_t* mem_bytes = (uint8_t *)memory;
    uint32_t registers[4]; 
    printf("\n");
    while(ip != end)
    {
        uint32_t value;
        uint8_t code, r0, r1, r2;
        uint16_t addr;
        _bytecode_read(*ip, &code, &value, &addr,
                &r0, &r1, &r2);
        printf("%d ", *ip);
        ip++;
        switch(code)
        {
            case ICONST:
                *(--stack) = value;
                break;
            case PUSH:
                *(--stack) = registers[r0];
                break; 
            case POP:
                registers[r0] =  *(stack++);
                break; 
            case TOP:
                registers[r0] = *stack;
                break;
            case LOAD:
                registers[r0] = *(uint32_t *)(mem_bytes + addr);
                break;
            case STORE:
                *(uint32_t *)(mem_bytes + addr) = registers[r0];
                break;
            case IADD:
            {
                int32_t a = *(int32_t *)&registers[r0];
                int32_t b = *(int32_t *)&registers[r1];
                int32_t res = a + b;
                printf("%d = %d\n", r1, res);
                registers[r2] = *(uint32_t *)&(res);
            } break;
            case ISUB:
            {
                int32_t a = *(int32_t *)&registers[r0];
                int32_t b = *(int32_t *)&registers[r1];
                int32_t res = a - b;
                registers[r2] = *(uint32_t *)&(res);
            } break;

            case GOTO:
                ip = instructions + value;
                break;

            case HALT:
                return *((int32_t *)&value);
        }
    }
    return 0;
}
