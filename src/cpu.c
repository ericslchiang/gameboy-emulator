#include "cpu.h"
#include "opcode.h"

CPU cpu;
uint32_t ticks;

void cpuInit(void) {
    cpu.IME = FALSE;
    cpu.af = 0x1180;
    cpu.bc = 0x0000;
    cpu.de = 0xFF56;
    cpu.hl = 0x000D;
    cpu.sp = 0xFFFE;
    // cpu.pc = 0x0000; // Boot ROM ends execution at 0x100, but it will hang if doesn't detect PPU interrupt signals
    cpu.pc = 0x0100; 
}

uint8_t cpuFetch(void) {
    return memoryRead(cpu.pc++);
}

void cpuExecuteOpcode(void) {
    uint8_t opcode = cpuFetch();
    // printf("%04X %04X %04X %04X %04X %04X \n", cpu.);
    printf("%04X: %02X\n", cpu.pc-1, opcode);
    // if (cpu.pc == 0xc6bb) printf("here");
    switch(opcode) {
        case 0xCB: cpuExecutePrefixOpcode(); break;
        case 0x00: break; //NOP
        case 0x01: LD(cpu.bc, cpuFetch() | cpuFetch() << 8); break;
        case 0x02: LD_MEM_8(cpu.bc, cpu.a); break;
        case 0x03: INC16(cpu.bc); break;
        case 0x04: INC8(&cpu.b); break;
        case 0x05: DEC8(&cpu.b); break;
        case 0x06: LD(cpu.b, cpuFetch()); break;
        // case 0x06: LD_8_MEM(cpu.b, cpuFetch()); break;
        // case 0x07: RL(&cpu.a, TRUE, FALSE); break;
        case 0x07: RL(&cpu.a, TRUE, TRUE); break;
        case 0x08: LD_MEM_SP(); break;
        case 0x09: ADD16(cpu.bc); break;
        case 0x0A: LD_8_MEM(cpu.a, cpu.bc); break;
        case 0x0B: DEC16(cpu.bc); break;
        case 0x0C: INC8(&cpu.c); break;
        case 0x0D: DEC8(&cpu.c); break;
        case 0x0E: LD(cpu.c, cpuFetch()); break;
        case 0x0F: RR(&cpu.a, TRUE, TRUE); break;
        case 0x10: STOP(); break;
        case 0x11: LD(cpu.de, cpuFetch() | cpuFetch() << 8); break;
        case 0x12: LD_MEM_8(cpu.de, cpu.a); break;
        case 0x13: INC16(cpu.de); break;
        case 0x14: INC8(&cpu.d); break;
        case 0x15: DEC8(&cpu.d); break;
        case 0x16: LD(cpu.d, cpuFetch()); break;
        // case 0x17: RL(&cpu.a, FALSE, FALSE); break;
        case 0x17: RL(&cpu.a, FALSE, TRUE); break;
        case 0x18: JR((int8_t)cpuFetch(), NOC); break;
        case 0x19: ADD16(cpu.de); break;
        case 0x1A: LD_8_MEM(cpu.a, cpu.de); break;
        case 0x1B: DEC16(cpu.de); break;
        case 0x1C: INC8(&cpu.e); break;
        case 0x1D: DEC8(&cpu.e); break;
        case 0x1E: LD(cpu.e, cpuFetch()); break;
        case 0x1F: RR(&cpu.a, FALSE, TRUE); break;
        case 0x20: JR((int8_t)cpuFetch(), NZ); break;
        case 0x21: LD(cpu.hl, cpuFetch() | cpuFetch() << 8); break;
        case 0x22: LD_MEM_8(cpu.hl++, cpu.a); break;
        case 0x23: INC16(cpu.hl); break;
        case 0x24: INC8(&cpu.h); break;
        case 0x25: DEC8(&cpu.h); break;
        case 0x26: LD(cpu.h, cpuFetch()); break;
        case 0x27: DAA(); break;
        case 0x28: JR((int8_t)cpuFetch(), Z); break;
        case 0x29: ADD16(cpu.hl); break;
        case 0x2A: LD_8_MEM(cpu.a, cpu.hl++); break;
        case 0x2B: DEC16(cpu.hl); break;
        case 0x2C: INC8(&cpu.l); break;
        case 0x2D: DEC8(&cpu.l); break;
        case 0x2E: LD(cpu.l, cpuFetch()); break;
        case 0x2F: CPL(); break;
        case 0x30: JR((int8_t)cpuFetch(), NC); break;
        case 0x31: LD(cpu.sp, cpuFetch() | cpuFetch() << 8); break;
        case 0x32: LD_MEM_8(cpu.hl--, cpu.a); break;
        case 0x33: INC16(cpu.sp); break;
        case 0x34: INC8(&memory.memory[cpu.hl]); break;
        case 0x35: DEC8(&memory.memory[cpu.hl]); break;
        case 0x36: LD_MEM_8(cpu.hl, cpuFetch()); break;
        case 0x37: SCF(); break;
        case 0x38: JR((int8_t)cpuFetch(), C); break;
        case 0x39: ADD16(cpu.sp); break;
        case 0x3A: LD_8_MEM(cpu.a, cpu.hl--); break;
        case 0x3B: DEC16(cpu.sp); break;
        case 0x3C: INC8(&cpu.a); break;
        case 0x3D: DEC8(&cpu.a); break;
        case 0x3E: LD(cpu.a, cpuFetch()); break;
        case 0x3F: CCF(); break;
        case 0x40: LD(cpu.b, cpu.b); break;
        case 0x41: LD(cpu.b, cpu.c); break;
        case 0x42: LD(cpu.b, cpu.d); break;
        case 0x43: LD(cpu.b, cpu.e); break;
        case 0x44: LD(cpu.b, cpu.h); break;
        case 0x45: LD(cpu.b, cpu.l); break;
        case 0x46: LD_8_MEM(cpu.b, cpu.hl); break;
        case 0x47: LD(cpu.b, cpu.a); break;
        case 0x48: LD(cpu.c, cpu.b); break;
        case 0x49: LD(cpu.c, cpu.c); break;
        case 0x4A: LD(cpu.c, cpu.d); break;
        case 0x4B: LD(cpu.c, cpu.e); break;
        case 0x4C: LD(cpu.c, cpu.h); break;
        case 0x4D: LD(cpu.c, cpu.l); break;
        case 0x4E: LD_8_MEM(cpu.c, cpu.hl); break;
        case 0x4F: LD(cpu.c, cpu.a); break;
        case 0x50: LD(cpu.d, cpu.b); break;
        case 0x51: LD(cpu.d, cpu.c); break;
        case 0x52: LD(cpu.d, cpu.d); break;
        case 0x53: LD(cpu.d, cpu.e); break;
        case 0x54: LD(cpu.d, cpu.h); break;
        case 0x55: LD(cpu.d, cpu.l); break;
        case 0x56: LD_8_MEM(cpu.d, cpu.hl); break;
        case 0x57: LD(cpu.d, cpu.a); break;
        case 0x58: LD(cpu.e, cpu.b); break;
        case 0x59: LD(cpu.e, cpu.c); break;
        case 0x5A: LD(cpu.e, cpu.d); break;
        case 0x5B: LD(cpu.e, cpu.e); break;
        case 0x5C: LD(cpu.e, cpu.h); break;
        case 0x5D: LD(cpu.e, cpu.l); break;
        case 0x5E: LD_8_MEM(cpu.e, cpu.hl); break;
        case 0x5F: LD(cpu.e, cpu.a); break;
        case 0x60: LD(cpu.h, cpu.b); break;
        case 0x61: LD(cpu.h, cpu.c); break;
        case 0x62: LD(cpu.h, cpu.d); break;
        case 0x63: LD(cpu.h, cpu.e); break;
        case 0x64: LD(cpu.h, cpu.h); break;
        case 0x65: LD(cpu.h, cpu.l); break;
        case 0x66: LD_8_MEM(cpu.h, cpu.hl); break;
        case 0x67: LD(cpu.h, cpu.a); break;
        case 0x68: LD(cpu.l, cpu.b); break;
        case 0x69: LD(cpu.l, cpu.c); break;
        case 0x6A: LD(cpu.l, cpu.d); break;
        case 0x6B: LD(cpu.l, cpu.e); break;
        case 0x6C: LD(cpu.l, cpu.h); break;
        case 0x6D: LD(cpu.l, cpu.l); break;
        case 0x6E: LD_8_MEM(cpu.l, cpu.hl); break;
        case 0x6F: LD(cpu.l, cpu.a); break;
        case 0x70: LD_MEM_8(cpu.hl, cpu.b); break;
        case 0x71: LD_MEM_8(cpu.hl, cpu.c); break;
        case 0x72: LD_MEM_8(cpu.hl, cpu.d); break;
        case 0x73: LD_MEM_8(cpu.hl, cpu.e); break;
        case 0x74: LD_MEM_8(cpu.hl, cpu.h); break;
        case 0x75: LD_MEM_8(cpu.hl, cpu.l); break;
        case 0x76: HALT(); break;
        case 0x77: LD_MEM_8(cpu.hl, cpu.a); break;
        case 0x78: LD(cpu.a, cpu.b); break;
        case 0x79: LD(cpu.a, cpu.c); break;
        case 0x7A: LD(cpu.a, cpu.d); break;
        case 0x7B: LD(cpu.a, cpu.e); break;
        case 0x7C: LD(cpu.a, cpu.h); break;
        case 0x7D: LD(cpu.a, cpu.l); break;
        case 0x7E: LD_8_MEM(cpu.a, cpu.hl); break;
        case 0x7F: LD(cpu.a, cpu.a); break;
        case 0x80: ADD8(&cpu.a, cpu.b); break;
        case 0x81: ADD8(&cpu.a, cpu.c); break;
        case 0x82: ADD8(&cpu.a, cpu.d); break;
        case 0x83: ADD8(&cpu.a, cpu.e); break;
        case 0x84: ADD8(&cpu.a, cpu.h); break;
        case 0x85: ADD8(&cpu.a, cpu.l); break;
        case 0x86: ADD8(&cpu.a, memoryRead(cpu.hl)); break;
        case 0x87: ADD8(&cpu.a, cpu.a); break;
        case 0x88: ADC(&cpu.a, cpu.b); break;
        case 0x89: ADC(&cpu.a, cpu.c); break;
        case 0x8A: ADC(&cpu.a, cpu.d); break;
        case 0x8B: ADC(&cpu.a, cpu.e); break;
        case 0x8C: ADC(&cpu.a, cpu.h); break;
        case 0x8D: ADC(&cpu.a, cpu.l); break;
        case 0x8E: ADC(&cpu.a, memoryRead(cpu.hl)); break;
        case 0x8F: ADC(&cpu.a, cpu.a); break;
        case 0x90: SUB(&cpu.a, cpu.b); break;
        case 0x91: SUB(&cpu.a, cpu.c); break;
        case 0x92: SUB(&cpu.a, cpu.d); break;
        case 0x93: SUB(&cpu.a, cpu.e); break;
        case 0x94: SUB(&cpu.a, cpu.h); break;
        case 0x95: SUB(&cpu.a, cpu.l); break;
        case 0x96: SUB(&cpu.a, memoryRead(cpu.hl)); break;
        case 0x97: SUB(&cpu.a, cpu.a); break;
        case 0x98: SBC(&cpu.a, cpu.b); break;
        case 0x99: SBC(&cpu.a, cpu.c); break;
        case 0x9A: SBC(&cpu.a, cpu.d); break;
        case 0x9B: SBC(&cpu.a, cpu.e); break;
        case 0x9C: SBC(&cpu.a, cpu.h); break;
        case 0x9D: SBC(&cpu.a, cpu.l); break;
        case 0x9E: SBC(&cpu.a, memoryRead(cpu.hl)); break;
        case 0x9F: SBC(&cpu.a, cpu.a); break;
        case 0xA0: AND(&cpu.a, cpu.b); break;
        case 0xA1: AND(&cpu.a, cpu.c); break;
        case 0xA2: AND(&cpu.a, cpu.d); break;
        case 0xA3: AND(&cpu.a, cpu.e); break;
        case 0xA4: AND(&cpu.a, cpu.h); break;
        case 0xA5: AND(&cpu.a, cpu.l); break;
        case 0xA6: AND(&cpu.a, memoryRead(cpu.hl)); break;
        case 0xA7: AND(&cpu.a, cpu.a); break;
        case 0xA8: XOR(&cpu.a, cpu.b); break;
        case 0xA9: XOR(&cpu.a, cpu.c); break;
        case 0xAA: XOR(&cpu.a, cpu.d); break;
        case 0xAB: XOR(&cpu.a, cpu.e); break;
        case 0xAC: XOR(&cpu.a, cpu.h); break;
        case 0xAD: XOR(&cpu.a, cpu.l); break;
        case 0xAE: XOR(&cpu.a, memoryRead(cpu.hl)); break;
        case 0xAF: XOR(&cpu.a, cpu.a); break;
        case 0xB0: OR(&cpu.a, cpu.b); break;
        case 0xB1: OR(&cpu.a, cpu.c); break;
        case 0xB2: OR(&cpu.a, cpu.d); break;
        case 0xB3: OR(&cpu.a, cpu.e); break;
        case 0xB4: OR(&cpu.a, cpu.h); break;
        case 0xB5: OR(&cpu.a, cpu.l); break;
        case 0xB6: OR(&cpu.a, memoryRead(cpu.hl)); break;
        case 0xB7: OR(&cpu.a, cpu.a); break;
        case 0xB8: CP(&cpu.a, cpu.b); break;
        case 0xB9: CP(&cpu.a, cpu.c); break;
        case 0xBA: CP(&cpu.a, cpu.d); break;
        case 0xBB: CP(&cpu.a, cpu.e); break;
        case 0xBC: CP(&cpu.a, cpu.h); break;
        case 0xBD: CP(&cpu.a, cpu.l); break;
        case 0xBE: CP(&cpu.a, memoryRead(cpu.hl)); break;
        case 0xBF: CP(&cpu.a, cpu.a); break;
        case 0xC0: RET(FALSE, NZ); break;
        case 0xC1: POP(&cpu.bc); break;
        case 0xC2: JP(cpuFetch() | cpuFetch() << 8, NZ); break;
        case 0xC3: JP(cpuFetch() | cpuFetch() << 8, NOC); break;
        case 0xC4: CALL(cpuFetch() | cpuFetch() << 8, NZ); break;
        case 0xC5: PUSH(cpu.bc); break;
        case 0xC6: ADD8(&cpu.a, cpuFetch()); break;
        case 0xC7: RST(0x00); break;
        case 0xC8: RET(FALSE, Z); break;
        case 0xC9: RET(FALSE, NOC); break;
        case 0xCA: JP(cpuFetch() | cpuFetch() << 8, Z); break;
        case 0xCC: CALL(cpuFetch() | cpuFetch() << 8, Z); break;
        case 0xCD: CALL(cpuFetch() | cpuFetch() << 8, NOC); break;
        case 0xCE: ADC(&cpu.a, cpuFetch()); break;
        case 0xCF: RST(0x08); break;
        case 0xD0: RET(FALSE, NC); break;
        case 0xD1: POP(&cpu.de); break;
        case 0xD2: JP(cpuFetch() | cpuFetch() << 8, NC); break;
        case 0xD4: CALL(cpuFetch() | cpuFetch() << 8, NC); break;
        case 0xD5: PUSH(cpu.de); break;
        case 0xD6: SUB(&cpu.a, cpuFetch()); break;
        case 0xD7: RST(0x10); break;
        case 0xD8: RET(FALSE, C); break;
        case 0xD9: RET(TRUE, NOC); break;
        case 0xDA: JP(cpuFetch() | cpuFetch() << 8, C); break;
        case 0xDC: CALL(cpuFetch() | cpuFetch() << 8, C); break;
        case 0xDE: SBC(&cpu.a, cpuFetch()); break;
        case 0xDF: RST(0x18); break;
        case 0xE0: LDH_MEM_A(); break;
        case 0xE1: POP(&cpu.hl); break;
        case 0xE2: LDH_C_A(); break;
        case 0xE5: PUSH(cpu.hl); break;
        case 0xE6: AND(&cpu.a, cpuFetch()); break;
        case 0xE7: RST(0x20); break;
        case 0xE8: ADD_SP(); break;
        case 0xE9: JP(cpu.hl, NOC); break;
        case 0xEA: LD_MEM_8(cpuFetch() | cpuFetch() << 8, cpu.a); break;
        case 0xEE: XOR(&cpu.a, cpuFetch()); break;
        case 0xEF: RST(0x28); break;
        case 0xF0: LDH_A_MEM(); break;
        case 0xF1: POP_AF(); break;
        case 0xF2: LDH_A_C(); break;
        case 0xF3: DI(); break;
        case 0xF5: PUSH_AF(); break;
        case 0xF6: OR(&cpu.a, cpuFetch()); break;
        case 0xF7: RST(0x30); break;
        case 0xF8: LD_HL_SP(); break;
        case 0xF9: LD(cpu.sp, cpu.hl); break;
        case 0xFA: LD_8_MEM(cpu.a, cpuFetch() | cpuFetch() << 8); break;
        case 0xFB: EI(); break;
        case 0xFE: CP(&cpu.a, cpuFetch()); break;
        case 0xFF: RST(0x38); break;
    }
    ticks += ClocksPerInstruction[opcode];
}

void cpuExecutePrefixOpcode(void) {
    uint8_t opcode = cpuFetch();
    uint8_t *reg;
    uint8_t isRegA = FALSE;

    switch(opcode & 0x07) {
        case 0: reg = &cpu.b; break;
        case 1: reg = &cpu.c; break;
        case 2: reg = &cpu.d; break;
        case 3: reg = &cpu.e; break;
        case 4: reg = &cpu.h; break;
        case 5: reg = &cpu.l; break;
        case 6: reg = &memory.memory[cpu.hl]; break;
        case 7: reg = &cpu.a; isRegA = FALSE; break;
    }

    switch(opcode & 0xF8) { // Opcode bitmask
        case 0x00: RL(reg, TRUE, isRegA); break;
        case 0x08: RR(reg, TRUE, isRegA); break;
        case 0x10: RL(reg, FALSE, isRegA); break;
        case 0x18: RR(reg, FALSE, isRegA); break;
        case 0x20: SLA(reg); break;
        case 0x28: SRA(reg); break;
        case 0x30: SWAP(reg); break;
        case 0x38: SRL(reg); break;
        case 0x40: BIT(reg, 0); break;
        case 0x48: BIT(reg, 1); break;
        case 0x50: BIT(reg, 2); break;
        case 0x58: BIT(reg, 3); break;
        case 0x60: BIT(reg, 4); break;
        case 0x68: BIT(reg, 5); break;
        case 0x70: BIT(reg, 6); break;
        case 0x78: BIT(reg, 7); break;
        case 0x80: RES(*reg, 0); break;
        case 0x88: RES(*reg, 1); break;
        case 0x90: RES(*reg, 2); break;
        case 0x98: RES(*reg, 3); break;
        case 0xA0: RES(*reg, 4); break;
        case 0xA8: RES(*reg, 5); break;
        case 0xB0: RES(*reg, 6); break;
        case 0xB8: RES(*reg, 7); break;
        case 0xC0: SET(*reg, 0); break;
        case 0xC8: SET(*reg, 1); break;
        case 0xD0: SET(*reg, 2); break;
        case 0xD8: SET(*reg, 3); break;
        case 0xE0: SET(*reg, 4); break;
        case 0xE8: SET(*reg, 5); break;
        case 0xF0: SET(*reg, 6); break;
        case 0xF8: SET(*reg, 7); break;
    }
    ticks += ClocksPerCBInstruction[opcode];
}

// Copied from emudev
// void cpuHandleTimer(uint32_t cycle) {

// }

void cpuISR(void) {
    uint8_t interruptFlag =  memoryRead(IF);
    uint8_t interruptEnable = memoryRead(IE);

    if (!cpu.IME || !interruptFlag) return;
    for (int8_t i = 4; i <= 0; i--) {
        if (getBit(i, interruptFlag)) {
            if (getBit(i, interruptEnable)) {
                // Perform the corresponding ISR
                cpu.IME = 0; // Disable global interrupt enable flag
                interruptFlag &= ~(1 << i); // Disable interrupt flag
                memoryWrite(0xFF0F, interruptFlag);

                // Push the PC onto the stack and jump to ISR in memory
                PUSH(cpu.pc);
                cpu.pc = ISR_Addr[i];

                // Add 5 M-cycles to the clock
                ticks += 20;
            }
        }
    }
}

void cpuIntRequest(uint8_t bitPos) {
    uint8_t interruptFlag = memoryRead(IF);
    interruptFlag |= (1 << bitPos);
    memoryWrite(IF, interruptFlag);
}

uint8_t getBit(uint8_t bitPos, uint8_t byte) {
    return byte & (1 << bitPos) ? 1 : 0;
}