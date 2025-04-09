#pragma once
#include <stdint.h>

#define FLAG_ZERO 7
#define FLAG_SUB 6
#define FLAG_HALF_CARRY 5
#define FLAG_CARRY 4

typedef struct {
    uint8_t IME; // Interrupt Master Enable
    uint8_t a;
    uint8_t f;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t pc;
    uint16_t sp;
} CPU;

void cpuInit(CPU *cpu);