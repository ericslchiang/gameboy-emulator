#include <stdio.h>
#include <stdio.h>
#include <string.h>

// #include "cpu.h"
// #include "ppu.h"
#include "memory.h"

// extern CPU cpu;

int main(void) {
  memoryLoadBootROM();
  memoryLoadCartridgeHeader();
  memoryLoadCartridge();
  return 0;
}