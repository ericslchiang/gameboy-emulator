#pragma once
#include <stdint.h>

enum EmulatorState {
    EMU_STOP,
    EMU_RUN,
};

typedef struct {
    uint64_t tCycles;
    uint64_t mCycles;
    uint8_t state;
} EMULATOR;

extern EMULATOR emulator;