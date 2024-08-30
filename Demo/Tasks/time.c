#include <stdint.h>
/* 
    __asm__ -> Inline assembly
    MCR -> Move to Coprocessor from ARM Registor
    p15 -> coprocessor number
    c9 -> PMU Performance Monitoring unit register
    c12, 0 -> control register from PMU
    %0 placeholder -> replaced by 1
    Writing 1 to the register enables it
    Define functions to access CPU registers
    c12, 2 -> register for cycle count 
    0x1 reset cycle count
    c12, 1 -> register to start and enable cycle count
    0x80000000 -> start cycle count
*/
void start_pmu_register(void) {
    // Start PMU
    __asm__ volatile("MCR p15, 0, %0, c9, c12, 0" : : "r"(1));
    // Reset cycle counter
    __asm__ volatile("MCR p15, 0, %0, c9, c12, 2" : : "r"(0x1));
    // Start cycle counter
    __asm__ volatile("MCR p15, 0, %0, c9, c12, 1" : : "r"(0x80000000));
}

uint32_t get_cycles(void) {
    uint32_t cycles;
    // Open registe, read the cycles in the register and store the cycles in cylcles
    __asm__ volatile("MRC p15, 0, %0, c9, c13, 0" : "=r"(cycles));
    return cycles;
}