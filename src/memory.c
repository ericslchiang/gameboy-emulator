#include "memory.h"

void memoryLoadBootROM(void) {
    FILE *filePointer = fopen("./roms/dmg_boot.bin", "rb");
    if(filePointer == NULL) {
        printf("Line %d: ", __LINE__ - 1);
        printf("\"Error loading Boot ROM\" in file %s\n", __FILE__);
        fclose(filePointer);
        return;
    }
    
    uint8_t byte;
    uint16_t i = 0;
    while (fread(&byte, sizeof(uint8_t), 1, filePointer) > 0) {
        memory.romBank0[i] = byte;
        i++;
    }
    fclose(filePointer);
}

void memoryLoadCartridge(void) {
    FILE *filePointer = fopen("./roms/tetris.gb", "rb"); // REPLACE GAME NAME WITH main argc VALUES
    if(filePointer == NULL) {
        printf("Line %d: ", __LINE__ - 1);
        printf("\"Error loading Boot ROM\" in file %s\n", __FILE__);
        return;
    }


// TODO:FIGURE OUT HOW TO LOAD CARTRIDGE DATA INTO MEMORY
    // uint8_t byte;
    // uint16_t i = 0x100;
    // while (fread(&byte, sizeof(uint8_t), 1, filePointer) > 0 && i < VRAM) {
    //     memory[i] = byte;        
    //     i++;
    // }
    fclose(filePointer);
}

void memoryVerifyCartridge(void) {
    char gameTitle[16] = {0}; 
    uint8_t oldLiceseeCode = 0, romSize = 0, ramSize = 0, headerChecksum = 0;
    uint16_t newLicenseeCode = 0;

    // Check if the cartridge's nintendo bitmap logo matches the bootrom's
    for (int i = 0; 0x104 < i < 0x133; i++) {
        if (memory.romBank0[i] != nintendoLogo[i - 0x104]){
            printf("Line %d: ", __LINE__ - 1);
            printf("\"Nintendo Bitmap logo mismatch\" in file %s\n", __FILE__);
        }
    } 

    for (int i = 0x134; i < 0x143; i++) {
        gameTitle[i] = memory.romBank0[i];
    }

    oldLiceseeCode = memory.romBank0[0x14B];
    if (oldLiceseeCode == 0x33)
        newLicenseeCode = memory.romBank0[0x144] | memory.romBank0[0x145] << 8; 

    switch (memory.romBank0[0x147]) {
        case 0x00: memory.mbcType = MBC_NULL; break;
        case 0x01: 
        case 0x02: 
        case 0x03: memory.mbcType = MBC_1; break;
        case 0x05:
        case 0x06: memory.mbcType = MBC_2; break;
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E: memory.mbcType = MBC_5; break;
    }

    romSize = memory.romBank0[0x148];
    switch (memory.romBank0[0x149]) { // RAM Bank is split into partitions of 8kB
        case 0x00:
        case 0x01: ramSize = 0; break;
        case 0x02: ramSize = 1; break;
        case 0x03: ramSize = 4; break;
        case 0x04: ramSize = 16; break;
        case 0x05: ramSize = 8; break;
    }
    headerChecksum = memory.romBank0[0x14D];

    // TODO: FIND OUT WHEN TO CALL FREE() ON MALLOC'd MEMORY
    memoryROMBank = (uint8_t **)malloc(1024 * 32 * (1 << romSize));
    memoryRAMBank = (uint8_t **)malloc(1024 * 8 * ramSize); 

    printf("Game Title: %s\n", gameTitle);
    printf("Licensee Code: %s\n", newLicenseeCode ? newLicenseeCode : oldLiceseeCode);
    printf("Cartridge Type: %d\n", memory.mbcType);
    printf("ROM Size: %d\n", romSize);
    printf("RAM Size: %d\n", memory.romBank0[0x149]);
    printf("Header Checksum: %d\n", headerChecksum);
}

uint8_t memoryRead(uint16_t address) {
    if (ROM_BANK_N < address < VRAM) { // Check if we are reading from memory bank

    }
    if (MEM_PROHIBITED < address < MEM_IO_REGISTERS ||
        RAM_ECHO < address < OAM
    ) {
        printf("Line %d, %s : \"Read from prohibited memory region: 0x%04X\"", __LINE__ - 1, __FILE__, address);
        return;
    }

    // TODO: Figure out checks for reading from memory
    return memory.memory[address];
}

void memoryWrite(uint16_t address, uint8_t byte) {
    if (address < VRAM) { // If writes to ROM check for MBC operations
        if (memory.mbcType == MBC_NULL) {
            printf("Line %d, %s : \"Prohibited write to ROM: 0x%04X\"", __LINE__ - 1, __FILE__, address);
            return;
        }
        memoryMBCWrite(address, byte);
    } else if (RAM_ECHO < address < OAM) { // Writes to ECHO_RAM will also write to WRAM
        memory.echo[address - RAM_ECHO] = byte;
        memory.memory[WRAM_0 + (address - RAM_ECHO)] = byte; //WRAM_0 + (address - RAM_ECHO) -> address - 0x2000 
    } else if (MEM_PROHIBITED < address < MEM_IO_REGISTERS) {
        printf("Line %d, %s : \"Write to prohibited memory region: 0x%04X\"", __LINE__ - 1, __FILE__, address);
        return;
    } else {
        memory.memory[address] = byte;
    } 
}

void memoryMBCWrite (uint16_t address, uint8_t byte) {
    switch(memory.mbcType) {
        case MBC_1:
            break;
        case MBC_2:
            break;
        case MBC_5:
            break;
    }
}