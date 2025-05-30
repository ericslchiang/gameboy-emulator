#pragma once
#include <stdint.h>
#include "memory.h"

static const uint8_t ISR_Addr[5] = {
    0x0040, // VBlank
    0x0048, // LCD Status
    0x0050, // Timer
    0x0058, // Serial Int
    0x0060, // Joypad
};

typedef struct {
    uint8_t IME : 1; // Interrupt Master Enable
    union {
        struct {
            union {
                uint8_t f;
                struct {
                    uint8_t padding : 4; // Lower unsed bit 0~3 of register f
                    uint8_t carry : 1; // bit 4
                    uint8_t halfCarry : 1; // bit 5
                    uint8_t sub : 1; // bit 6
                    uint8_t zero : 1; // bit 7
                };
            };
            uint8_t a;
        };
        uint16_t af;
    };
    union {
        struct {
            uint8_t c;
            uint8_t b;
        };
        uint16_t bc;
    };
    union {
        struct {
            uint8_t e;
            uint8_t d;
        };
        uint16_t de;
    };
    union {
        struct {
            uint8_t l;
            uint8_t h;
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

extern CPU cpu;
extern uint32_t ticks;

void cpuInit(void);
uint8_t cpuFetch(void);
void cpuExecuteOpcode(void);
void cpuExecutePrefixOpcode(void);
void cpuISR(void);
void cpuIntRequest(uint8_t bitPos);
static uint8_t getBit(uint8_t bitPos, uint8_t byte);