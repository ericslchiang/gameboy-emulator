#pragma once
#include <stdint.h>
#include <stdio.h>

#define ROM_BANK_0 0x0000
#define ROM_BANK_N 0x4000
#define VRAM 0x8000
#define RAM_EXTERNAL 0xA000
#define WRAM_0 0xC000
#define WRAM_N 0xD000
#define RAM_ECHO 0xE000
#define OAM 0xFE00
#define MEM_PROHIBITED 0xFEA0
#define MEM_IO_REGISTERS 0xFF00
#define HRAM 0xFF80
#define MEM_INTERRUPT_EN 0xFFFF

//IO Defines
#define IO_INTERRUPT 0XFF0F

typedef struct {
    uint8_t memory[0xFFFF];
} MEMORY;

MEMORY memory;

void memoryLoadBootROM(void);
uint8_t memoryRead(uint16_t address);
void memoryWrite(uint16_t address, uint8_t byte);