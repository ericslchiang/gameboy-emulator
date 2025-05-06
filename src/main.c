#include <stdio.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"
// #include "ppu.h"
#include "memory.h"

// extern CPU cpu;

int main(void) {
  memoryLoadCartridgeHeader();
  memoryLoadCartridge();
  memoryLoadBootROM();
  cpuInit();

  while (1) {
    cpuExecuteOpcode();
  }
  return 0;
}