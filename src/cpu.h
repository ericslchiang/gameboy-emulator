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

static const uint8_t ClocksPerInstruction[] = {
//  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	4,  12, 8,  8,  4,  4,  8,  4,  20, 8,  8,  8,  4,  4,  8,  4,  // 00
	4,  12, 8,  8,  4,  4,  8,  4,  12, 8,  8,  8,  4,  4,  8,  4,  // 10
	8,  12, 8,  8,  4,  4,  8,  4,  8,  8,  8,  8,  4,  4,  8,  4,  // 20
	8,  12, 8,  8,  12, 12, 12, 4,  8,  8,  8,  8,  4,  4,  8,  4,  // 30
	4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 40
	4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 50
	4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 60
	8,  8,  8,  8,  8,  8,  4,  8,  4,  4,  4,  4,  4,  4,  8,  4,  // 70
	4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 80
	4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 90
	4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // A0
	4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // B0
	8,  12, 12, 16, 12, 16, 8,  16, 8,  16, 12, 4,  12, 24, 8,  16, // C0
	8,  12, 12, 0,  12, 16, 8,  16, 8,  16, 12, 0,  12, 0,  8,  16, // D0
	12, 12, 8,  0,  0,  16, 8,  16, 16, 4,  16, 0,  0,  0,  8,  16, // E0
	12, 12, 8,  4,  0,  16, 8,  16, 12, 8,  16, 4,  0,  0,  8,  16, // F0
};

static const uint8_t ClocksPerCBInstruction[] = {
//  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8, // 00
	8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8, // 10
	8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8, // 20
	8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8, // 30
	8,  8,  8,  8,  8,  8,  12, 8,  8,  8,  8,  8,  8,  8,  12, 8, // 40
	8,  8,  8,  8,  8,  8,  12, 8,  8,  8,  8,  8,  8,  8,  12, 8, // 50
	8,  8,  8,  8,  8,  8,  12, 8,  8,  8,  8,  8,  8,  8,  12, 8, // 60
	8,  8,  8,  8,  8,  8,  12, 8,  8,  8,  8,  8,  8,  8,  12, 8, // 70
	8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8, // 80
	8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8, // 90
	8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8, // A0
	8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8, // B0
	8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8, // C0
	8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8, // D0
	8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8, // E0
	8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8, // F0
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
uint8_t getBit(uint8_t bitPos, uint8_t byte);