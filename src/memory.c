#include "memory.h"

extern MEMORY memory;

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
        memory.memory[i] = byte;
        i++;
    }
    fclose(filePointer);
}

uint8_t memoryRead(uint16_t address) {
    if (MEM_PROHIBITED < address < MEM_IO_REGISTERS) {
        printf("Line %d, %s : \"Read from prohibited memory region\"", __LINE__ - 1, __FILE__);
        return;
    }

    return memory.memory[address];
}

void memoryWrite(uint16_t address, uint8_t byte) {
    if (MEM_PROHIBITED < address < MEM_IO_REGISTERS) {
        printf("Line %d, %s : \"Read from prohibited memory region\"", __LINE__ - 1, __FILE__);
        return;
    }

    memory.memory[address] = byte;
}