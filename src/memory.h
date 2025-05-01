#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emulator.h>

#define ROM_BANK_0 0x0000
#define ROM_BANK_N 0x4000
#define VRAM 0x8000
#define RAM_BANK 0xA000
#define WRAM_0 0xC000
#define WRAM_N 0xD000
#define RAM_ECHO 0xE000
#define OAM 0xFE00
#define MEM_PROHIBITED 0xFEA0
#define MEM_IO_REGISTERS 0xFF00
#define HRAM 0xFF80
#define MEM_INTERRUPT_EN 0xFFFF

static const uint8_t nintendoLogo[] = {
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
};

enum MBC {
    MBC_NULL,
    MBC_1, // No support for multi-game carts (MBC1M)
    MBC_2,
    MBC_3,
    MBC_5,
};

typedef struct {
    union {  
        struct {
            uint8_t romBank0[ROM_BANK_N - ROM_BANK_0];
            uint8_t romBankN[VRAM - ROM_BANK_N];
            uint8_t vRAM[RAM_BANK - VRAM];
            uint8_t ramBank[WRAM_0 - RAM_BANK];
            uint8_t wRam0[WRAM_N - WRAM_0];
            uint8_t wRamN[RAM_ECHO - WRAM_N];
            uint8_t echo[OAM - RAM_ECHO];
            uint8_t oam[MEM_PROHIBITED - OAM];
            uint8_t prohibited[MEM_IO_REGISTERS - MEM_PROHIBITED];
            uint8_t ioReg[HRAM - MEM_IO_REGISTERS];
            uint8_t hram[0xFFFE - HRAM];
            uint8_t interrupt[1];
        };
        uint8_t memory[0xFFFF];
    };
} MEMORY;

/*
    currentRAMBank: 2-bit register
    bankMode: For MBC_1, this 1-bit register selects between ROM Banking (0) and RAM Banking mode (1), 
        the program may freely switch between the two modes.
        ROM Banking Mode = 8KB RAM, 2MB ROM (default)
        RAM Banking Mode = 32KB RAM, 512KB ROM
*/
typedef struct {
    uint8_t mbcType;
    uint8_t numROMBank;
    uint8_t numRAMBank;
    uint8_t currentROMBank;
    uint8_t currentRAMBank; 
    uint8_t externalRAMEnable;
    uint8_t bankMode;
} CARTRIDGE;

static uint8_t *memoryROMBank;
static uint8_t *memoryRAMBank;

extern EMULATOR emulator;

void memoryLoadBootROM(void);
uint8_t memoryLoadCartridge(void);
void memoryLoadCartridgeHeader(void);
uint8_t memoryRead(uint16_t address);
void memoryWrite(uint16_t address, uint8_t byte);
void memoryMBC1Write(uint16_t address, uint8_t byte);
void memoryMBC2Write(uint16_t address, uint8_t byte);
void memoryMBC3Write(uint16_t address, uint8_t byte);
void memoryMBC5Write(uint16_t address, uint8_t byte);
