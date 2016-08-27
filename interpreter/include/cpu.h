#ifndef CPU_H_
#define CPU_H_
#include <stdint.h>

int cpu_execute(uint32_t* instructions, int instructions_size,
        uint32_t* memory, int memory_size,
        int entry_point);
#endif
