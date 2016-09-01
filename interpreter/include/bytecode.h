#ifndef BYTECODE_H_
#define BYTECODE_H_

#include <stdint.h>
#include <assert.h>

/* prefixes:
 * i = i32
 * u = ui32
 * l = i64
 * ul = ui64
 * s = i16
 * us = ui16
 * c = i8
 * b = ui8
 * f = single float 
 * d = double float
 * p = pointer (32 bits)
 */

#define REG_I32_A 0
#define REG_I32_B 1
#define REG_I32_C 2
#define REG_I32_D 3

// Pushes a constant onto the stack 
#define ICONST 0

// Push the value in a register onto the stack
// as x bytes
#define PUSH 1

// Pops x bytes value from the stack
#define POP 2

// Stores x bytes from an offset of the top of 
// the stack into a register
#define TOP 3

// Sets the instruction pointer to a value
#define GOTO 4

// halts the program
#define HALT 5

// Retrieves a from memory into a register
#define LOAD 6

// stores a value from a register into memory
#define STORE 7

// Pops an ip stored on the top of the stacks 
// into an i32 register
#define POPIP 8

// Sets the ip to the value in an I32 register 
#define JMP 9

// Sets the value on the top of the stack to ip
#define PUSHIP 10

// Push a stack frame and jump to the value in an I32 register
#define CALL 11

// Pops a stack frame and jumps back to the address pushed with CALL
#define RET 12

// Adds the values from two registers 
// and stores the result into a register
#define IADD 22

// Subtracts the values from two registers 
// and stores the result into a register
#define ISUB 23
//
// Adds the values from two registers 
// and stores the result into a register
#define IMULT 24

// Subtracts the values from two registers 
// and stores the result into a register
#define IDIV 25

#include <stdio.h>

static inline uint32_t _bytecode_write(uint8_t code,
        uint32_t value, uint16_t addr,
        uint8_t r0, uint8_t r1, uint8_t r2)
{
    return (uint32_t)code << 24 | (value & 0x00FFFFFF) |
        (uint32_t)r0 << 16 | (uint32_t)r1 << 8 | (uint32_t)r2 | addr;
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

static inline uint32_t bytecode_iconst(int32_t value)
{
    return _bytecode_write(ICONST, *((uint32_t *)&value), 0, 0, 0, 0);
}

/* r0: register to push
 * r1: bytecount
 */
static inline uint32_t bytecode_push(uint8_t r0, uint8_t r1)
{
    assert(r1 > 0 && r1 <=4);
    return _bytecode_write(PUSH, 0, 0, r0, r1, 0);
}

/* r0: register to store into
 * r1: bytecount
 */
static inline uint32_t bytecode_pop(uint8_t r0, uint8_t r1)
{
    assert(r1 > 0 && r1 <=4);
    return _bytecode_write(POP, 0, 0, r0, r1, 0);
}

/* r0: register to store into
 * r1: bytecount
 * r2: offset
 */
static inline uint32_t bytecode_top(uint8_t r0, uint8_t r1, uint8_t r2)
{
    assert(r1 > 0 && r1 <=4);
    return _bytecode_write(TOP, 0, 0, r0, r1, r2);
}

static inline uint32_t bytecode_load(uint8_t r0, uint16_t addr)
{
    return _bytecode_write(LOAD, 0, addr, r0, 0, 0);
}

static inline uint32_t bytecode_store(uint8_t r0, uint16_t addr)
{
    return _bytecode_write(STORE, 0, addr, r0, 0, 0);
}

static inline uint32_t bytecode_goto(uint16_t addr)
{
    return _bytecode_write(GOTO, 0, addr, 0, 0, 0);
}

static inline uint32_t bytecode_popip(uint8_t r0)
{
    return _bytecode_write(POPIP, 0, 0, r0, 0, 0);
}

static inline uint32_t bytecode_puship()
{
    return _bytecode_write(PUSHIP, 0, 0, 0, 0, 0);
}

static inline uint32_t bytecode_jmp(uint8_t r0)
{
    return _bytecode_write(JMP, 0, 0, r0, 0, 0);
}

static inline uint32_t bytecode_call(uint8_t r0)
{
    return _bytecode_write(CALL, 0, 0, r0, 0, 0);
}

static inline uint32_t bytecode_ret()
{
    return _bytecode_write(RET, 0, 0, 0, 0, 0);
}

static inline uint32_t bytecode_iadd(uint8_t r0, uint8_t r1, uint8_t r2)
{
    return _bytecode_write(IADD, 0, 0, r0, r1, r2);
}

static inline uint32_t bytecode_isub(uint8_t r0, uint8_t r1, uint8_t r2)
{
    return _bytecode_write(ISUB, 0, 0, r0, r1, r2);
}

static inline uint32_t bytecode_imult(uint8_t r0, uint8_t r1, uint8_t r2)
{
    return _bytecode_write(IMULT, 0, 0, r0, r1, r2);
}

static inline uint32_t bytecode_idiv(uint8_t r0, uint8_t r1, uint8_t r2)
{
    return _bytecode_write(IDIV, 0, 0, r0, r1, r2);
}

static inline uint32_t bytecode_halt(int return_value)
{
    return _bytecode_write(HALT, *(uint32_t *)&return_value, 0, 0, 0, 0);
}

#endif
