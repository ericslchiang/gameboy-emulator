#pragma once
#include <stdint.h>

enum EmulatorState {

};

typedef struct {
    uint64_t tCycles;
    uint64_t mCycles;
    uint8_t state;
} EMULATOR;

EMULATOR emulator;