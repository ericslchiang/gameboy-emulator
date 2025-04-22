#pragma once
#include <stdint.h>

CPU cpu;

#define FLAG_ZERO 7
#define FLAG_SUB 6
#define FLAG_HALF_CARRY 5
#define FLAG_CARRY 4

typedef struct {
    uint8_t IME; // Interrupt Master Enable
    union {
        struct {
            uint8_t a;
            uint8_t f;
        };
        uint16_t af;
    };
    union {
        struct {
            uint8_t b;
            uint8_t c;
        };
        uint16_t bc;
    };
    union {
        struct {
            uint8_t d;
            uint8_t e;
        };
        uint16_t de;
    };
    union {
        struct {
            uint8_t h;
            uint8_t l;
        };
        uint16_t hl;
    };
    uint16_t pc;
    uint16_t sp;
} CPU;

// // Maybe use for readability
// typedef uint8_t u8;
// typedef uint16_t u16;
// typedef int8_t i8;
// typedef int16_t i16;

void cpuInit(void);
static uint8_t cpuFetch(void);
void cpuExecuteOpcode(void);
void cpuExecutePrefixOpcode(void);