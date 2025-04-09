#include "opcode.h"

void ADD8(uint8_t *reg, uint8_t val) {
    uint8_t before = *reg;
    *reg += val;
    
    if ((before & 0xF) + (val & 0xF) > 0xF) // Check for half byte overflow
        cpu.f |= 1 << FLAG_HALF_CARRY; 
    if (before + val > 0xFF) // Check for byte overflow
        cpu.f |= 1 << FLAG_CARRY;
    if (*reg == 0) // Check if result is 0
        cpu.f |= 1 << FLAG_ZERO;
    cpu.f &= ~(1 << FLAG_SUB);
}

void SUB(uint8_t *reg, uint8_t val) {
    uint8_t before = *reg;
    *reg -= val;
    
    if ((before & 0xF) < (val & 0xF)) // Check for half byte underflow
        cpu.f |= 1 << FLAG_HALF_CARRY; 
    if (before < val) // Check for byte underflow
        cpu.f |= 1 << FLAG_CARRY;
    if (*reg == 0) // Check if result is 0
        cpu.f |= 1 << FLAG_ZERO;
    cpu.f |= 1 << FLAG_SUB;
}

void ADD16(uint16_t *reg, uint16_t val) {
    uint16_t before = *reg;
    *reg += val;

    if(*reg > 0xFFFF) //Check for overflow
        cpu.f |= 1 << FLAG_CARRY;
    if((before & 0xFFF) + (val & 0xFFF) > 0xFFF) //Check for overflow on 11th bit
        cpu.f |= 1 << FLAG_HALF_CARRY;
    cpu.f &= ~(1 << FLAG_SUB);
}

void ADC(uint8_t *reg, uint8_t val) {
    uint8_t before = *reg;
    uint8_t carry = cpu.f & (1 << FLAG_CARRY) ? 1 : 0;
    *reg += val + carry;

    if ((before & 0xF) + (val & 0xF) + carry > 0xF)
        cpu.f |= 1 << FLAG_HALF_CARRY;
    if (before + val + carry > 0xFF)
        cpu.f |= 1 << FLAG_CARRY;
    if (*reg == 0)
        cpu.f |= 1 << FLAG_ZERO;
    cpu.f &= ~(1 << FLAG_SUB);
}

void SBC(uint8_t *reg, uint8_t val) {
    uint8_t before = *reg;
    uint8_t carry = cpu.f & (1 << FLAG_CARRY) ? 1 : 0;
    *reg -= val + carry;

    if ((before & 0xF) < (val & 0xF) + carry)
        cpu.f |= 1 << FLAG_HALF_CARRY;
    if (before < val + carry)
        cpu.f |= 1 << FLAG_CARRY;
    if (*reg == 0)
        cpu.f |= 1 << FLAG_ZERO;
    cpu.f |= 1 << FLAG_SUB;
}

void CP(uint8_t *reg, uint8_t val){
    uint8_t before = *reg;
    
    if ((before & 0xF) < (val & 0xF)) // Check for half byte underflow
        cpu.f |= 1 << FLAG_HALF_CARRY; 
    if (before < val) // Check for byte underflow
        cpu.f |= 1 << FLAG_CARRY;
    if (before == val) // Check if result is 0
        cpu.f |= 1 << FLAG_ZERO;
    cpu.f |= 1 << FLAG_SUB;
}

void INC8(uint8_t *reg) {
    uint8_t before = *reg;
    *reg++;

    if (before & 0xF == 0xF)
        cpu.f |= 1 << FLAG_HALF_CARRY;
    if (*reg == 0)
        cpu.f |= 1 << FLAG_ZERO;
    cpu.f &= ~(1 << FLAG_SUB);
}

void DEC8(uint8_t *reg) {
    uint8_t before = *reg;
    *reg--;

    if (before & 0xF == 0x0)
        cpu.f |= 1 << FLAG_HALF_CARRY;
    if (*reg == 0)
        cpu.f |= 1 << FLAG_ZERO;
    cpu.f |= 1 << FLAG_SUB;
}

void AND(uint8_t *reg, uint8_t val) {
    *reg &= val;
    cpu.f &= ~(1 << FLAG_SUB | 1 << FLAG_CARRY);
    cpu.f |= 1 << FLAG_HALF_CARRY;
    if(*reg == 0) 
        cpu.f |= 1 << FLAG_ZERO;
}

void OR(uint8_t *reg, uint8_t val) {
    *reg |= val;
    cpu.f &= ~(1 << FLAG_CARRY | 1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (*reg == 0)
        cpu.f |= 1 << FLAG_ZERO;
}

void XOR(uint8_t *reg, uint8_t val) {
    *reg ^= val;
    cpu.f &= ~(1 << FLAG_CARRY | 1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (*reg == 0)
        cpu.f |= 1 << FLAG_ZERO;
}

void CPL(uint8_t *reg) {
    *reg = ~(*reg);
    cpu.f |= 1 << FLAG_HALF_CARRY | 1 << FLAG_SUB;
}

void BIT(uint8_t *reg, uint8_t bitPosition) {
    if (*reg & (1 << bitPosition) == 0)
        cpu.f |= 1 << FLAG_ZERO;
    cpu.f |= 1 << FLAG_HALF_CARRY;
    cpu.f &= ~(1 << FLAG_SUB);
}

void RL(uint8_t *reg, uint8_t isCircular, uint8_t isRegA) {
    uint8_t before = *reg;
    *reg = *reg << 1 | isCircular ? (before >> 7) : ((cpu.f >> FLAG_CARRY) & 1);
    cpu.f = (before >> 0x7) << FLAG_CARRY;

    cpu.f &= ~(1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (!isRegA && *reg == 0) 
        cpu.f |= 1 << FLAG_ZERO;
    else
        cpu.f &= ~(1 << FLAG_ZERO);
}

void RR(uint8_t *reg, uint8_t isCircular, uint8_t isRegA) {
    uint8_t before = *reg;
    *reg = *reg >> 1 | isCircular ? (before & 0x80) : ((cpu.f >> FLAG_CARRY) << 7);
    cpu.f = (before & 1) << FLAG_CARRY;

    cpu.f &= ~(1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (!isRegA && *reg == 0) 
        cpu.f |= 1 << FLAG_ZERO;
    else
        cpu.f &= ~(1 << FLAG_ZERO);
}

void SLA(uint8_t *reg) {
    cpu.f = (*reg >> 7) << FLAG_CARRY;
    *reg <<= 1;

    cpu.f &= ~(1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (*reg == 0) 
        cpu.f |= 1 << FLAG_ZERO;
    else
        cpu.f &= ~(1 << FLAG_ZERO);
}

void SRA(uint8_t *reg) {
    cpu.f = (*reg & 1) << FLAG_CARRY;
    *reg = *reg >> 1 | (*reg & 0x80);

    cpu.f &= ~(1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (*reg == 0) 
        cpu.f |= 1 << FLAG_ZERO;
    else
        cpu.f &= ~(1 << FLAG_ZERO);
}

void SRL(uint8_t *reg) {
    cpu.f = (*reg & 1) << FLAG_CARRY;
    *reg >>= 1;

    cpu.f &= ~(1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (*reg == 0) 
        cpu.f |= 1 << FLAG_ZERO;
    else
        cpu.f &= ~(1 << FLAG_ZERO);
}

void SWAP(uint8_t *reg) {
    uint8_t upperNibble = (*reg & 0b11110000) >> 4;
    *reg = *reg << 4 | upperNibble;
    cpu.f &= ~(1 << FLAG_CARRY | 1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (*reg == 0)
        cpu.f |= 1 << FLAG_ZERO;
}

void JP(uint16_t address, enum ConditionCode cc) {
    if (checkConditionCode(cc))
        cpu.pc = address;
}

void CALL(uint16_t address, enum ConditionCode cc) {
    if(checkConditionCode(cc)) {
        memoryWrite(--cpu.sp, (uint8_t)((cpu.pc + 1) >> 8)); //Decrement stack address, write MSByte to stack
        memoryWrite(--cpu.sp, (uint8_t)((cpu.pc + 1) & 0xFF)); //Decrement stack address, write LSByte to stack
        JP(address, NOC); // Perform jump operation
    }
}

void JR(uint8_t offset, enum ConditionCode cc) {
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
    cpu.f ^= 1 << FLAG_CARRY;
    cpu.f &= ~(1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
}

void SCF(void) {
    cpu.f |= 1 << FLAG_CARRY;
    cpu.f &= ~(1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
}

void DI(void) {
    cpu.IME = 0;
}

void EI(void) {
    cpu.IME = 1;
}

void HALT(void) {
    if (cpu.IME) {
        //Enter Low Power Mode
    } else {
        if (memoryRead(MEM_INTERRUPT_EN) & memoryRead(IO_INTERRUPT)) { // Some Interrupts Pending
            //CPU continues execution, but reads the next byte twice due to hardware bug
        } else { //No interrupts pending

        }
    }
}

void DAA(void) {
    uint8_t adjustment = 0;
    if (cpu.f & (1 << FLAG_SUB)) {
        adjustment += (cpu.f & (1 << FLAG_HALF_CARRY)) ? 0x6 : 0;
        adjustment += (cpu.f & (1 << FLAG_CARRY)) ? 0x60 : 0;
        cpu.a -= adjustment;
    } else {
        if (cpu.f & (1 << FLAG_HALF_CARRY) || cpu.a & 0xF > 0x9)
            adjustment += 0x6;
        if (cpu.f & (1 << FLAG_CARRY) || cpu.a > 0x99) {
            adjustment += 0x60;
            cpu.f |= 1 << FLAG_CARRY;
        }
        cpu.a += adjustment;
    }

    if (cpu.a == 0)
        cpu.f |= 1 << FLAG_ZERO;
    cpu.f &= ~(1 << FLAG_HALF_CARRY);
}

void NOP(void) {;}
void STOP(void) {
    //Not sure if need to implement anything
}

static uint8_t checkConditionCode(enum ConditionCode cc) {
    switch (cc) {
        case Z:
            return cpu.f & (1 << FLAG_ZERO) ? 1 : 0; break;
        case NZ:
            return cpu.f & (1 << FLAG_ZERO) ? 0 : 1; break;
        case C:
            return cpu.f & (1 << FLAG_CARRY) ? 1 : 0; break;
        case NC:
            return cpu.f & (1 << FLAG_CARRY) ? 0 : 1; break;
        case NOC:
        default:
            return 1; break;
    }
}