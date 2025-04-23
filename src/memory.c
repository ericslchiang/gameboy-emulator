#include "memory.h"

MEMORY memory;
CARTRIDGE cartridge = {0}; // ROM Bank should always be 1. ROM_BANK_0 is always loaded in memory

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

uint8_t memoryLoadCartridge(void) {
    FILE *filePointer = fopen("./roms/tetris.gb", "rb"); // REPLACE GAME NAME WITH main argc VALUES
    if(filePointer == NULL) {
        printf("Line %d: ", __LINE__ - 1);
        printf("\"Error loading Boot ROM\" in file %s\n", __FILE__);
        fclose(filePointer);
        return 0;
    }

    // Read cartridge into ROM Bank 0
    if (fread(memory.romBank0, sizeof(uint8_t), ROM_BANK_N, filePointer) != ROM_BANK_N){
        printf("Line %d: Error reading into ROM Bank 0 in file %s\n", __LINE__ - 1, __FILE__);
        fclose(filePointer);
        return 0;
    }

    // Read ROM content into ROM bank
    uint8_t byte;
    uint16_t i = 0;
    fseek(filePointer, ROM_BANK_N, SEEK_SET);
    while (fread(&byte, sizeof(uint8_t), 1, filePointer) > 0) {
        memoryROMBank[i] = byte;
        i++;
    }

    fclose(filePointer);
    return 1;
}

void memoryLoadCartridgeHeader(void) {
    uint8_t cartridgeHeader[0x50] = {0}; // Cartridge header is located in the rom file from 0x100 ~ 0x14F

    FILE *filePointer = fopen("./roms/tetris.gb", "rb"); // REPLACE GAME NAME WITH main argc VALUES
    if(filePointer == NULL) {
        printf("Line %d: Error opening ROM in file %s\n", __LINE__ - 1, __FILE__);
        fclose(filePointer);
        return;
    }

    fseek(filePointer, 0x100, SEEK_SET); // Set filestream position to start of header values
    for (int i = 0; i < 0x50; i++) {
        cartridgeHeader[i] = fgetc(filePointer);
    }
    fclose(filePointer);

    char gameTitle[16] = {0}; 
    uint8_t headerChecksum = 0;
    uint16_t licenseeCode = 0;

    // Check if the cartridge's nintendo bitmap logo matches the bootrom's
    for (int i = 0x04; i < 0x34; i++) {
        if (cartridgeHeader[i] != nintendoLogo[i - 0x04]){
            printf("Line %d: ", __LINE__ - 1);
            printf("\"Nintendo Bitmap logo mismatch\" in file %s\n", __FILE__);
        }
    } 

    for (int i = 0x34; i < 0x43; i++) {
        gameTitle[i] = (char)cartridgeHeader[i];
    }

    licenseeCode = cartridgeHeader[0x4B];
    if (licenseeCode == 0x33)
        licenseeCode = cartridgeHeader[0x44] | cartridgeHeader[0x45] << 8; 

    switch (cartridgeHeader[0x147]) {
        case 0x00: cartridge.mbcType = MBC_NULL; break;
        case 0x01: 
        case 0x02: 
        case 0x03: cartridge.mbcType = MBC_1; break;
        case 0x05:
        case 0x06: cartridge.mbcType = MBC_2; break;
        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13: cartridge.mbcType = MBC_3; break;
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E: cartridge.mbcType = MBC_5; break;
    }

    cartridge.numROMBank = 1 << cartridgeHeader[0x48] + 1;
    switch (cartridgeHeader[0x49]) { // RAM Bank is split into partitions of 8kB
        case 0x00:
        case 0x01: cartridge.numRAMBank = 0; break;
        case 0x02: cartridge.numRAMBank = 1; break;
        case 0x03: cartridge.numRAMBank = 4; break;
        case 0x04: cartridge.numRAMBank = 16; break;
        case 0x05: cartridge.numRAMBank = 8; break;
    }
    headerChecksum = cartridgeHeader[0x4D];

    // TODO: FIND OUT WHEN TO CALL FREE() ON MALLOC'd MEMORY
    memoryROMBank = (uint8_t *)calloc(1024 * 16 * cartridge.numROMBank, sizeof(uint8_t));
    memoryRAMBank = (uint8_t *)calloc(1024 * 8 * cartridge.numRAMBank, sizeof(uint8_t)); 
    
    printf("Game Title: %s\n", gameTitle);
    printf("Licensee Code: %d\n", licenseeCode);
    printf("Cartridge Type: %d\n", cartridge.mbcType);
    printf("ROM Size: %d banks\n", cartridge.numROMBank);
    printf("RAM Size: %d banks\n", cartridge.numRAMBank);
    printf("Header Checksum: %d\n", headerChecksum);
}

uint8_t memoryRead(uint16_t address) {
    if (ROM_BANK_N < address < VRAM) { // Check if we are reading from memory bank

    }
    if (MEM_PROHIBITED < address < MEM_IO_REGISTERS ||
        RAM_ECHO < address < OAM
    ) {
        printf("Line %d, %s : \"Read from prohibited memory region: 0x%04X\"", __LINE__ - 1, __FILE__, address);
        return 0;
    }

    // TODO: Figure out checks for reading from memory
    return memory.memory[address];
}

void memoryWrite(uint16_t address, uint8_t byte) {
    if (address < VRAM) { // If writes to ROM check for MBC operations
        if (cartridge.mbcType == MBC_NULL) {
            printf("Line %d, %s : \"Prohibited write to ROM: 0x%04X\"", __LINE__ - 1, __FILE__, address);
            return;
        }

        // External RAM enable
        if (address < 0x2000 && (byte & 0x0F) == 0x0A) {
            cartridge.memoryExternalRAMEnable = 1;
        } else if (0x2000 < address < ROM_BANK_N) { // Set active ROM bank
            if (byte & 0x1F == 0x00) { // If ROM bank 0 is selected, automatically change to ROM bank 1
                cartridge.currentROMBank = 0x01; 
            } else {
                if (cartridge.numROMBank > 0x1F) { // If ROM bank is larger than 5 bits, grab 2 other bits from currentRAMBank register
                    cartridge.currentROMBank = (cartridge.currentRAMBank << 5) + byte;
                } else { 
                    uint8_t romBankMask = 0x1F;
                    while (byte & romBankMask > cartridge.numROMBank) { // If requested bank is larger than number of ROM banks, mask leftmost
                                                                        // non-zero bit until requested bank is smaller than number of ROM banks
                        romBankMask >>= 1; 
                    }
                    cartridge.currentROMBank = byte & romBankMask;
                }
                // TODO: Theres some finicky issues with MBC types where the above code in incorrect
            }
        } else if (ROM_BANK_N < address < 0x6000) { // Set selected RAM bank 
            cartridge.currentRAMBank = byte & 0x03;
        } else if (0x6000 < address < VRAM) { // Sets banking mode
            cartridge.bankMode = byte & 0x01; 
        }

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
    switch(cartridge.mbcType) {
        case MBC_1:
            break;
        case MBC_2:
            break;
        case MBC_5:
            break;
    }
}