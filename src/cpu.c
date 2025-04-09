#include "cpu.h"

extern CPU cpu;

void cpuInit(CPU *cpu) {
    cpu->IME = 0;
    cpu->a = 0X01;
    cpu->f = 0XB0;
    cpu->b = 0X00;
    cpu->c = 0X13;
    cpu->d = 0X00;
    cpu->e = 0XD8;
    cpu->h = 0X01;
    cpu->l = 0X4D;
    cpu->sp = 0xFFFE;
    cpu->pc = 0x0100;
}