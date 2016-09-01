#include "../include/cpu.h"
#include "../include/bytecode.h"
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

inline uint32_t bitfield(int bytecount)
{
    switch(bytecount)
    {
        case(1):
            return 0x000000FF;
        case(2):
            return 0x0000FFFF;
        case(3):
            return 0x00FFFFFF;
        case(4):
            return 0xFFFFFFFF;
        default:
            printf("unexpected bytecount %d\n", bytecount);
            assert(0);
    }
}

int cpu_execute(uint32_t* instructions, int instructions_size,
        uint8_t* memory, int memory_bytes,
        int entry_point)
{
    uint32_t *end = instructions + instructions_size;
    uint8_t *stack = memory + memory_bytes;
    uint32_t *ip = instructions + entry_point;

    uint32_t registers[4]; 

    printf("\n");
    printf("%d instructions: %p\n", instructions_size, instructions);
    while(ip != end)
    {
        uint32_t value;
        uint8_t code, r0, r1, r2;
        uint16_t addr;
        _bytecode_read(*ip, &code, &value, &addr,
                &r0, &r1, &r2);
        printf("[%d] %8x\n", (int)(ip - instructions), *ip);
        printf("  c: %d r0: %d r1: %d r2: %d addr: %d value: %d\n  ", 
                code, r0, r1, r2, addr, value);

        uint8_t *begin = stack;
        int i = 0;
        for(; begin != memory + memory_bytes; i++)
        {
            printf("%02x ", *begin);
            if ((i % 16) == 15)
            {
                printf("\n  ");
            }
            begin++;
        }
        printf("\n");
        switch(code)
        {
            case ICONST:
                stack-=4;
                *((uint32_t* )stack) = value;
                break;
            case PUSH:
                stack-=r1;
                *((uint32_t* )stack) = registers[r0] & bitfield(r1);
                break; 
            case POP:
                {
                    registers[r0] = ((*(uint32_t *)(stack)) & bitfield(r1));
                    stack += r1;
                }
                break; 
            case POPIP:
                {
                    registers[r0] = ((*(uint32_t *)(stack)));
                    stack+=4;
                } break;
            case PUSHIP:
                {
                    stack-=4;
                    *((uint32_t* )stack) = (uint32_t)((ip - instructions));
                } break;
            case JMP:
                {
                    ip = instructions + registers[r0];
                } break;
            case CALL:
                {
                    stack-=4;
                    *((uint32_t* )stack) = (uint32_t)((ip - instructions));
                    ip = instructions + registers[r0];
                } break;
            case RET:
                {
                    ip = (instructions + (*(uint32_t *)(stack)));
                    stack+=4;
                } break;
            case TOP:
                registers[r0] = (*(uint32_t *)(stack + r1) & bitfield(r2));
                break;
            case LOAD:
                registers[r0] = *(uint32_t *)(memory + addr);
                break;
            case STORE:
                *(uint32_t *)(memory + addr) = registers[r0];
                break;
            case IADD:
            {
                int32_t a = *(int32_t *)&registers[r0];
                int32_t b = *(int32_t *)&registers[r1];
                int32_t res = a + b;
                registers[r2] = *(uint32_t *)&(res);
            } break;
            case ISUB:
            {
                int32_t b = *(int32_t *)&registers[r0];
                int32_t a = *(int32_t *)&registers[r1];
                int32_t res = a - b;
                registers[r2] = *(uint32_t *)&(res);
            } break;
            case IMULT:
            {
                int32_t a = *(int32_t *)&registers[r0];
                int32_t b = *(int32_t *)&registers[r1];
                int32_t res = a * b;
                registers[r2] = *(uint32_t *)&(res);
            } break;
            case IDIV:
            {
                int32_t b = *(int32_t *)&registers[r0];
                int32_t a = *(int32_t *)&registers[r1];
                int32_t res = a / b;
                registers[r2] = *(uint32_t *)&(res);
            } break;

            case GOTO:
                ip = instructions + addr;
                break;

            case HALT:
                return *((int32_t *)&value);

            default:
                printf("Unhandled instruction [%d]: %d", code, *ip);
        }
        ip++;
    }
    return 0;
}
