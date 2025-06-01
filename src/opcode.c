#include "opcode.h"

void LD_MEM_SP(void) {
    uint16_t address = cpuFetch() | cpuFetch() << 8;
    memoryWrite(address, (uint8_t)(cpu.sp & 0xFF));
    memoryWrite(++address, (uint8_t)(cpu.sp >> 8));
}

// void LD_8_MEM(uint8_t *reg, uint16_t address) {
//     *reg = memoryRead(address); 
// }

// void LD_MEM_8(uint16_t address, uint8_t reg) {
//     memoryWrite(address, reg);
// }

void LD_HL_SP(void) {
    int8_t val = cpuFetch();
    uint16_t before = cpu.sp;

    cpu.hl = (uint16_t)(before + val);

    cpu.zero = 0;
    cpu.sub = 0;
    cpu.carry = (val & 0xFF) + (before & 0xFF) > 0xFF ? 1 : 0; //Overflow from bit 7 
    cpu.halfCarry = (val & 0xF) + (before & 0xF) > 0xF ? 1 : 0; //Overflow from bit 3
}

void ADD8(uint8_t *reg, uint8_t val) {
    uint8_t before = *reg;
    *reg += val;
    
    cpu.zero = (*reg == 0) ? 1 : 0;
    cpu.sub = 0;
    cpu.carry = (before + val > 0xFF) ? 1 : 0; // Overflow on bit 7
    cpu.halfCarry = (before & 0xF) + (val & 0xF) > 0xF ? 1 : 0; // Overflow on bit 3
}

void SUB(uint8_t *reg, uint8_t val) {
    uint8_t before = *reg;
    *reg -= val;

    cpu.zero = (*reg == 0) ? 1 : 0;
    cpu.sub = 1;
    cpu.carry = val > before ? 1 : 0; // Check for byte underflow
    cpu.halfCarry = (val & 0xF) > (before & 0xF) ? 1 : 0; // Check for half byte underflow
}

void ADD_SP(void) {
    uint16_t before = cpu.sp;
    int8_t val = cpuFetch();
    cpu.sp += val;

    cpu.zero = 0;
    cpu.sub = 0;
    cpu.carry = (before & 0xFF) + val > 0xFF ? 1 : 0; //Check for overflow on 7th bit
    cpu.halfCarry = (before & 0xF) + (int8_t)(val & 0xF) > 0xF ? 1 : 0; //Check for overflow on 3rd bit
}

void ADD16(uint16_t val) {
    uint16_t before = cpu.hl; // Used excusively for HL register
    cpu.hl += val;

    cpu.sub = 0;
    cpu.carry = before + val > 0xFFFF ? 1 : 0; // Overflow on 15th bit
    cpu.halfCarry = (before & 0xFFF) + (val & 0xFFF) > 0xFFF ? 1 : 0; //Check for overflow on 11th bit    
}

void ADC(uint8_t *reg, uint8_t val) {
    uint8_t before = *reg;
    *reg += val + cpu.carry;

    cpu.zero = (*reg == 0) ? 1 : 0;
    cpu.sub = 0; 
    cpu.halfCarry = (before & 0xF) + (val & 0xF) + cpu.carry > 0xF ? 1 : 0;
    cpu.carry = before + val + cpu.carry > 0xFF ? 1 : 0;
}

void SBC(uint8_t *reg, uint8_t val) {
    uint8_t before = *reg;
    *reg -= val + cpu.carry;

    cpu.zero = (*reg == 0) ? 1 : 0;
    cpu.sub = 1; 
    cpu.halfCarry = (val & 0xF) + cpu.carry > (before & 0xF) ? 1 : 0;
    cpu.carry = val + cpu.carry > before ? 1 : 0;
}

void CP(uint8_t *reg, uint8_t val){
    uint8_t before = *reg;
    
    cpu.zero = (before == val) ? 1 : 0;
    cpu.sub = 1; 
    cpu.halfCarry = (val & 0xF) > (before & 0xF) ? 1 : 0;
    cpu.carry = val > before ? 1 : 0;
}

void INC8(uint8_t *reg) {
    uint8_t before = *reg;
    (*reg)++;

    cpu.zero = (*reg == 0) ? 1 : 0;
    cpu.sub = 0; 
    cpu.halfCarry = ((before & 0xF) == 0xF) ? 1 : 0;
}

void DEC8(uint8_t *reg) {
    uint8_t before = *reg;
    (*reg)--;

    cpu.zero = (*reg == 0) ? 1 : 0;
    cpu.sub = 1; 
    cpu.halfCarry = ((before & 0xF) == 0) ? 1 : 0;
}

void AND(uint8_t *reg, uint8_t val) {
    *reg &= val;

    cpu.zero = (*reg == 0) ? 1 : 0;
    cpu.sub = 0;
    cpu.halfCarry = 1;
    cpu.carry = 0;
}

void OR(uint8_t *reg, uint8_t val) {
    *reg |= val;

    cpu.zero = (*reg == 0) ? 1 : 0;
    cpu.sub = 0;
    cpu.halfCarry = 0;
    cpu.carry = 0;
}

void XOR(uint8_t *reg, uint8_t val) {
    *reg ^= val;
    
    cpu.zero = (*reg == 0) ? 1 : 0;
    cpu.sub = 0;
    cpu.halfCarry = 0;
    cpu.carry = 0;
}

// Only works on register A
void CPL(void) {
    cpu.a = ~(cpu.a);
    
    cpu.sub = 0;
    cpu.halfCarry = 0;
}

void BIT(uint8_t *reg, uint8_t bitPosition) {
    if ((*reg) & (1 << bitPosition) == 0)
        cpu.zero = 0;

    cpu.sub = 0;
    cpu.halfCarry = 0;
}

void RL(uint8_t *reg, uint8_t isCircular, uint8_t isRegA) {
    uint8_t before = *reg;
    if (isCircular) {
        cpu.carry = before >> 7; // Carry flag set to bit 7 of register
        *reg = (*reg << 1) + cpu.carry; // Register rotated by 1 bit
    } else {
        *reg = (*reg << 1) + cpu.carry; // Register rotated by 1 bit including carry flag  
        cpu.carry = before >> 7; // Carry flag set to bit 7 of register
    }

    cpu.zero = (*reg == 0) || (isRegA == 0) ? 1 : 0;
    cpu.sub = 0;
    cpu.halfCarry = 0;
}

void RR(uint8_t *reg, uint8_t isCircular, uint8_t isRegA) {
    uint8_t before = *reg;
    if (isCircular) {
        cpu.carry = before & 0x01; // Carry flag set to bit 0 of register
        *reg = (cpu.carry << 7) + (*reg >> 1); // Register rotated by 1 bit
    } else {
        *reg = (cpu.carry << 7) + (*reg >> 1); // Register rotated by 1 bit including carry flag
        cpu.carry = before & 0x01; // Carry flag set to bit 0 of register
    }

    cpu.zero = (*reg == 0) || (isRegA == 0) ? 1 : 0;
    cpu.sub = 0;
    cpu.halfCarry = 0;
}

void SLA(uint8_t *reg) {
    cpu.carry = *reg >> 7;
    *reg = *reg << 1;

    cpu.zero = (*reg == 0) ? 1 : 0;
    cpu.sub = 0;
    cpu.halfCarry = 0;
}

void SRA(uint8_t *reg) {
    cpu.carry = *reg & 0x01;
    *reg = *reg >> 1 + (*reg & 0x80);

    cpu.zero = (*reg == 0) ? 1 : 0;
    cpu.sub = 0;
    cpu.halfCarry = 0;
}

void SRL(uint8_t *reg) {
    cpu.carry = *reg & 0x01;
    *reg >>= 1;

    cpu.zero = (*reg == 0) ? 1 : 0;
    cpu.sub = 0;
    cpu.halfCarry = 0;
}

void SWAP(uint8_t *reg) {
    uint8_t upperNibble = *reg & 0xF0; 
    uint8_t lowerNibble = *reg & 0x0F;
    *reg = lowerNibble << 4 | upperNibble >> 4;

    cpu.zero = (*reg == 0) ? 1 : 0;
    cpu.sub = 0;
    cpu.halfCarry = 0;
    cpu.carry = 0;
}

void JP(uint16_t address, enum ConditionCode cc) {
    if (checkConditionCode(cc))
        cpu.pc = address;
}

void CALL(uint16_t address, enum ConditionCode cc) {
    if(checkConditionCode(cc)) {
        memoryWrite(--cpu.sp, (uint8_t)((cpu.pc) >> 8)); //Decrement stack address, write MSByte to stack
        memoryWrite(--cpu.sp, (uint8_t)((cpu.pc) & 0xFF)); //Decrement stack address, write LSByte to stack
        JP(address, NOC); // Perform jump operation
    }
}

void JR(int8_t offset, enum ConditionCode cc) {
    if(checkConditionCode(cc)) {
        JP(cpu.pc + offset, NOC);
    }
}

void RET(uint8_t enableInterrupt, enum ConditionCode cc) {
    if (checkConditionCode(cc)) {
        uint16_t retValue = 0;
        retValue |= memoryRead(cpu.sp++); //Set lower byte
        retValue |= memoryRead(cpu.sp++) << 8; //Set upper byte
        cpu.pc = retValue;

        if (enableInterrupt)
            EI();
    }
}

void RST(uint8_t vec) {
    PUSH(cpu.pc);
    cpu.pc = vec;
}

void POP(uint16_t *reg) {
    *reg = memoryRead(cpu.sp++);
    *reg |= memoryRead(cpu.sp++) << 8;
}

void PUSH(uint16_t reg) {
    memoryWrite(--cpu.sp, (uint8_t)(reg >> 8)); //Decrement stack address, write MSByte to stack
    memoryWrite(--cpu.sp, (uint8_t)(reg & 0xFF)); //Decrement stack address, write LSByte to stack
}

void CCF(void) {
    cpu.carry = ~cpu.carry;
    cpu.sub = 0;
    cpu.halfCarry = 0;
}

void SCF(void) {
    cpu.carry = 1;
    cpu.sub = 0;
    cpu.halfCarry = 0;
}

void DI(void) {
    cpu.IME = FALSE;
}

void EI(void) {
    cpu.IME = TRUE;
}

void HALT(void) {
    if (cpu.IME) {
        //Enter Low Power Mode
    } else {
        if (memoryRead(IE) & memoryRead(0xFF00)) { // Some Interrupts Pending
            //CPU continues execution, but reads the next byte twice due to hardware bug
        } else { //No interrupts pending

        }
    }
}

void DAA(void) {
    uint8_t adjustment = 0;
    if (cpu.sub) {
        adjustment += cpu.halfCarry ? 0x6 : 0;
        adjustment += cpu.carry ? 0x60 : 0;
        cpu.a -= adjustment;
    } else {
        if (cpu.halfCarry || (cpu.a & 0xF) > 0x9)
            adjustment += 0x6;
        if (cpu.carry || cpu.a > 0x99) {
            adjustment += 0x60;
            cpu.carry = 1;
        }
        cpu.a += adjustment;
    }

    cpu.zero = (cpu.a == 0) ? 1 : 0;
    cpu.halfCarry = 0;
}

void NOP(void) {}   
void STOP(void) {
    //Not sure if need to implement anything
}

static uint8_t checkConditionCode(enum ConditionCode cc) {
    switch (cc) {
        case Z:
            return cpu.zero ? 1 : 0; break;
        case NZ:
            return cpu.zero ? 0 : 1; break;
        case C:
            return cpu.carry ? 1 : 0; break;
        case NC:
            return cpu.carry ? 0 : 1; break;
        case NOC:
        default:
            return 1; break;
    }
}
