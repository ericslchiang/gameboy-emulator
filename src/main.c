#include <stdio.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "ppu.h"
#include "memory.h"

CPU cpu;
MEMORY memory;

int main(void) {
  memoryLoadBootROM();
  return 0;
}