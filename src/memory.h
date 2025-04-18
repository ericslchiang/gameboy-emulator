#pragma once
#include <stdint.h>
#include <stdio.h>

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

//IO Defines
#define IO_INTERRUPT 0XFF0F

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

MEMORY memory;

void memoryLoadBootROM(void);
void memoryLoadCartridge(void);
void memoryVerifyCartridge(void);
uint8_t memoryRead(uint16_t address);
void memoryWrite(uint16_t address, uint8_t byte);