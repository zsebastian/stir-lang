#ifndef BYTECODE_H_
#define BYTECODE_H_

#include <stdint.h>

// Pushes a constant onto the stack 
#define ICONST 0

// Push the value in a register onto the stack
#define PUSH 1

// Pops a value from the stack
#define POP 2

// Stores the value on top of the stack into 
// a register
#define TOP 3

// Retrieves a value from memory into a register
#define LOAD 4

// Stores a value from a register into memory
#define STORE 5

// Adds the values from two registers 
// and stores the result into a register
#define IADD 6

// Subtracts the values from two registers 
// and stores the result into a register
#define ISUB 7

// Sets the instruction pointer to a value
#define GOTO 8

// halts the program
#define HALT 9

#include <stdio.h>

static inline uint32_t _bytecode_write(uint8_t code,
        uint32_t value, uint16_t addr,
        uint8_t r0, uint8_t r1, uint8_t r2)
{
    uint32_t ret = (uint32_t)code << 24 | (value & 0x00FFFFFF) |
        (uint32_t)r0 << 16 | (uint32_t)r1 << 8 | (uint32_t)r2 | addr;
    printf("%d ", ret);
    return ret;
}

static inline void _bytecode_read(uint32_t bytecode, uint8_t *code,
        uint32_t *value, uint16_t *addr,
        uint8_t *r0, uint8_t *r1, uint8_t *r2)
{
    *code = (uint8_t)(bytecode >> 24);
    *value = (uint32_t)(bytecode & 0x00FFFFF);
    *addr = (uint16_t)(bytecode & 0x0000FFFF);
    *r0 = (uint16_t)((bytecode & 0x00FF0000) >> 16);
    *r1 = (uint16_t)((bytecode & 0x0000FF00) >> 8);
    *r2 = (uint16_t)((bytecode & 0x000000FF));
}

static uint32_t bytecode_iconst(int32_t value)
{
    return _bytecode_write(ICONST, *((uint32_t *)&value), 0, 0, 0, 0);
}

static uint32_t bytecode_push(uint8_t r0)
{
    return _bytecode_write(PUSH, 0, 0, r0, 0, 0);
}

static uint32_t bytecode_pop(uint8_t r0)
{
    return _bytecode_write(POP, 0, 0, r0, 0, 0);
}

static uint32_t bytecode_top(uint8_t r0)
{
    return _bytecode_write(TOP, 0, 0, r0, 0, 0);
}

static uint32_t bytecode_load(uint8_t r0, uint16_t addr)
{
    return _bytecode_write(LOAD, 0, addr, r0, 0, 0);
}

static uint32_t bytecode_store(uint8_t r0, uint16_t addr)
{
    return _bytecode_write(STORE, 0, addr, r0, 0, 0);
}

static uint32_t bytecode_iadd(uint8_t r0, uint8_t r1, uint8_t r2)
{
    return _bytecode_write(IADD, 0, 0, r0, r1, r2);
}

static uint32_t bytecode_isub(uint8_t r0, uint8_t r1, uint8_t r2)
{
    return _bytecode_write(ISUB, 0, 0, r0, r1, r2);
}

static uint32_t bytecode_halt(int return_value)
{
    return _bytecode_write(HALT, *(uint32_t *)&return_value, 0, 0, 0, 0);
}

#endif
