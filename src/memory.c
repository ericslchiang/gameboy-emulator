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
    FILE *filePointer = fopen("./roms/tetris.bin", "rb"); // REPLACE GAME NAME WITH main argc VALUES
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
    uint8_t oldLiceseeCode = 0, cartridgeType = 0, romSize = 0, ramSize = 0, headerChecksum = 0;
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

    cartridgeType = memory.romBank0[0x147];
    romSize = memory.romBank0[0x148];
    ramSize = memory.romBank0[0x149];
    headerChecksum = memory.romBank0[0x14D];

    printf("Game Title: %s\n", gameTitle);
    printf("Licensee Code: %s\n", newLicenseeCode ? newLicenseeCode : oldLiceseeCode);
    printf("Cartridge Type: %d\n", cartridgeType);
    printf("ROM Size: %d\n", romSize);
    printf("RAM Size: %d\n", ramSize);
    printf("Header Checksum: %d\n", headerChecksum);
}

uint8_t memoryRead(uint16_t address) {
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
    // Prohibiting writes to ROM, ECHO_RAM, and Restricted area in memory
    if (MEM_PROHIBITED < address < MEM_IO_REGISTERS || 
            ROM_BANK_0 < address < VRAM ||
            RAM_ECHO < address < OAM
    ) {
        printf("Line %d, %s : \"Write to prohibited memory region: 0x%04X\"", __LINE__ - 1, __FILE__, address);
        return;
    }

    // Mirroring writes to 0xC000 ~ 0xDDFF into ECHO_RAM
    if (WRAM_0 < address < 0xDDFF) {
        memory.echo[address - WRAM_0] = byte;
    }

    // TODO: Figure out checks for writing to memory
    memory.memory[address] = byte;
}