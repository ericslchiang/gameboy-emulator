#include "memory.h"
#define ROM_NAME "./roms/10-bit ops.gb"

MEMORY memory;
CARTRIDGE cartridge = {
    .mbcType = 0,
    .numROMBank = 0,
    .numRAMBank = 0,
    .currentROMBank = 1, // ROM Bank should always be 1. ROM_BANK_0 is always loaded in memory
    .currentRAMBank = 0, 
    .externalRAMEnable = 0,
    .bankMode = 0, 
}; 

void memoryLoadBootROM(void) {
    FILE *filePointer = fopen("./roms/dmg_boot.bin", "rb");
    if(filePointer == NULL) {
        printf("Line %d: ", __LINE__ - 1);
        printf("\"Error loading Boot ROM\" in file %s\n", __FILE__);
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
    FILE *filePointer = fopen(ROM_NAME, "rb"); // REPLACE GAME NAME WITH main argc VALUES
    if(filePointer == NULL) {
        printf("Line %d: ", __LINE__ - 1);
        printf("\"Error loading Boot ROM\" in file %s\n", __FILE__);
        return 0;
    }
    
    // Read cartridge into ROM bank
    uint8_t byte;
    uint32_t i = 0;
    while (fread(&byte, sizeof(uint8_t), 1, filePointer) > 0) {
        memoryROMBank[i++] = byte;
    }
    fclose(filePointer);

    // Read ROM Bank 0 into static CPU ROM
    memcpy(memory.romBank0, memoryROMBank, ROM_BANK_N);
    
    // Copy ROM bank 1 into CPU Memory
    // memcpy(memory.romBankN, memoryROMBank, sizeof(memory.romBankN)); 
    /*
    Not sure if copying an entire ROM Bank into CPU memory is the best way
    Maybe in memoryRead() we just read from ROM Bank memory instead. FIGURE THIS OUT. 
    */
    return 1;
}

void memoryLoadCartridgeHeader(void) {
    uint8_t cartridgeHeader[0x50] = {0}; // Cartridge header is located in the rom file from 0x100 ~ 0x14F

    FILE *filePointer = fopen(ROM_NAME, "rb"); // REPLACE GAME NAME WITH main argc VALUES
    if(filePointer == NULL) {
        printf("Line %d: Error opening ROM in file %s\n", __LINE__ - 1, __FILE__);
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
    memoryROMBank = calloc(1024 * 16 * cartridge.numROMBank, sizeof(uint8_t)); // Each ROM bank is 16 kilobytes
    if (cartridge.mbcType == MBC_2) {
        memoryRAMBank = calloc(512, sizeof(uint8_t)); // MBC_2 has built in RAM of 512 half-bytes
    } else {
        memoryRAMBank = calloc(1024 * 8 * cartridge.numRAMBank, sizeof(uint8_t)); 
    }
    
    printf("Game Title: %s\n", gameTitle);
    printf("Licensee Code: %d\n", licenseeCode);
    printf("Cartridge Type: %d\n", cartridge.mbcType);
    printf("ROM Size: %d banks\n", cartridge.numROMBank);
    printf("RAM Size: %d banks\n", cartridge.numRAMBank);
    printf("Header Checksum: %d\n", headerChecksum);
}

uint8_t memoryRead(uint16_t address) {
    // Increment clock
    emulator.mCycles++;
    emulator.tCycles+=4;

    if (ROM_BANK_N <= address && address < VRAM) { // Check if we are reading from ROM bank
        return memoryROMBank[(cartridge.currentROMBank - 1) * 16 * 1024 + address];
    } else if (RAM_BANK <= address && address < WRAM_0) { // Check if we are reading from external RAM Bank
        if (cartridge.externalRAMEnable) {
            return memoryRAMBank[cartridge.currentRAMBank * 1024 * 8 + address - RAM_BANK];
        } else {
            printf("Error trying to read external RAM when it it disabled");
            return 0xFF;
        }
    } else if (RAM_ECHO <= address && address < OAM) { // Return value from WRAM_0
        return memory.wRam0[address - RAM_ECHO];
    } else if (MEM_PROHIBITED <= address && address < MEM_IO_REGISTERS) {
        printf("Line %d, %s : \"Read from prohibited memory region: 0x%04X\"", __LINE__ - 1, __FILE__, address);
        return 0;
    } else {
        return memory.memory[address];
    }
}

void memoryWrite(uint16_t address, uint8_t byte) {
    // Increment clock
    emulator.mCycles++;
    emulator.tCycles+=4;
    
    if (address == 0xFF02 && byte == 0x81) {
        // Emulate serial output
        printf("%d", memoryRead(0xFF01));
        return;
    } 

    if (address < VRAM) { // If writes to ROM check for MBC operations
        switch (cartridge.mbcType) {
            case MBC_1: memoryMBC1Write(address, byte); break;
            case MBC_2: memoryMBC2Write(address, byte); break;
            case MBC_3: memoryMBC3Write(address, byte); break;
            case MBC_5: memoryMBC5Write(address, byte); break;
            case MBC_NULL: 
                printf("Line %d, %s : \"Prohibited write to ROM (MBC_NULL): 0x%04X\"", __LINE__ - 1, __FILE__, address); 
                break;        
        }
    } else if (RAM_BANK <= address && address < WRAM_0 && cartridge.externalRAMEnable) { // Check if external RAM is enabled before writing to it
        memoryRAMBank[cartridge.currentRAMBank * 8 * 1024 + address - RAM_BANK] = byte;
    } else if (RAM_ECHO <= address && address < OAM) { // Writes to ECHO_RAM will also write to WRAM
        memory.echo[address - RAM_ECHO] = byte;
        memory.memory[WRAM_0 + (address - RAM_ECHO)] = byte; //WRAM_0 + (address - RAM_ECHO) -> address - 0x2000 
    } else if (MEM_PROHIBITED <= address && address < MEM_IO_REGISTERS) {
        printf("Line %d, %s : \"Write to prohibited memory region: 0x%04X\"", __LINE__ - 1, __FILE__, address);
    } else {
        memory.memory[address] = byte;
    } 
}

void memoryMBC1Write (uint16_t address, uint8_t byte) {
    // External RAM enable/disable
    if (address < 0x2000) {
        cartridge.externalRAMEnable = byte & 0x0F == 0x0A ? 1 : 0; // The byte value being written has to be 0x0A to enable external RAM
    } else if (0x2000 < address && address < ROM_BANK_N) { // Set active ROM bank
        // If the selected ROM Bank # is higher that then the number of ROM Banks, the MSB is masked
        uint8_t bankMask = 0x1F;
        while ((byte & bankMask) > cartridge.numROMBank)
            bankMask >>= 1;

        cartridge.currentROMBank &= 0b11100000;
        cartridge.currentROMBank |= byte & bankMask;
        if (cartridge.bankMode == 0) cartridge.currentRAMBank++;
    } else if (ROM_BANK_N <= address && address < 0x6000) { // Set selected RAM bank 
        if (cartridge.bankMode == 0) { // Change bit 5 & 6 of current ROM Bank
            cartridge.currentROMBank &= 0x1F;
            cartridge.currentROMBank |= byte & 0b11100000;
            if (cartridge.currentROMBank == 0) cartridge.currentROMBank++;
        } else { // Change curent RAM Bank
            cartridge.currentRAMBank = byte & 0x03;
        }
    } else if (0x6000 <= address && address < VRAM) { // Sets banking mode
        cartridge.bankMode = byte & 0x01;
        if (cartridge.bankMode == 0) cartridge.currentRAMBank++; 
    }
}

void memoryMBC2Write (uint16_t address, uint8_t byte) {
    if (address < ROM_BANK_N) { 
        if ((address & 0x0100) == 0) { // If bit 8 of the address is 0, control external RAM
            cartridge.externalRAMEnable = (byte & 0x0F) == 0x0A ? 1 : 0; // External RAM is enabled if lower nibble of written vaue is 0xA
        } else { // If bit 8 of the address is 0, control selected ROM Bank
            cartridge.currentROMBank = byte & 0x0F;
            if (cartridge.currentROMBank == 1) cartridge.currentROMBank++;
        }
    } else {
        printf("Undefined MBC_2 behaviour writing to address: %04X\n", address);
    }
}

void memoryMBC3Write(uint16_t address, uint8_t byte){}
void memoryMBC5Write(uint16_t address, uint8_t byte){}