#include <stdio.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"
// #include "ppu.h"
#include "memory.h"
#include "ppu.h"

// extern CPU cpu;

int main(void) {
  memoryLoadCartridgeHeader();
  memoryLoadCartridge();
  memoryLoadBootROM();
  memoryInitRegisters();
  cpuInit();

  while (1) {
    cpuExecuteOpcode();
    // ppuUpdate(); 
    /*
    A overview of the PPU system:
    The PPU renders graphics independently of the CPU. In fact, the CPU which counts on M-cycles (~1MHz), runs
    slower than the PPU, which runs at ~2Mhz. Each period of the PPU clock is referred to as a "dot".

    As such, a seperate counter is required for the PPU. However, as the CPU drives opcodes and memory acceses,
    M-cycles will determine the counter inside of the PPU. Each M-cycle update should also trigger a call to 
    a PPU cycle update function, which then determines if the alloted amount of time has been given to each PPU 
    operation mode. 

    The emulator struct holds all timers. Current idea is to update M-cycles inside of emulator, then the emulator 
    will translate that amount of time to PPU cycles, which is then passes to an "updatePPUFunction". Unknown if
    async code will be required. 
    */
  }
  return 0;
}