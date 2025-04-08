#include "opcode.h"

void ADD8(uint8_t *reg, uint8_t val, uint8_t *flags) {
    uint8_t before = *reg;
    *reg += val;
    
    if ((before & 0xF) + (val & 0xF) > 0xF) // Check for half byte overflow
        *flags |= 1 << FLAG_HALF_CARRY; 
    if (before + val > 0xFF) // Check for byte overflow
        *flags |= 1 << FLAG_CARRY;
    if (*reg == 0) // Check if result is 0
        *flags |= 1 << FLAG_ZERO;
    *flags &= ~(1 << FLAG_SUB);
}

void SUB(uint8_t *reg, uint8_t val, uint8_t *flags) {
    uint8_t before = *reg;
    *reg -= val;
    
    if ((before & 0xF) < (val & 0xF)) // Check for half byte underflow
        *flags |= 1 << FLAG_HALF_CARRY; 
    if (before < val) // Check for byte underflow
        *flags |= 1 << FLAG_CARRY;
    if (*reg == 0) // Check if result is 0
        *flags |= 1 << FLAG_ZERO;
    *flags |= 1 << FLAG_SUB;
}

void ADD16(uint8_t *upper_reg, uint8_t *lower_reg, uint16_t val, uint8_t *flags) {
    uint16_t before = *upper_reg << 8 + *lower_reg;
    uint16_t after = before + val;

    *lower_reg = after & 0xFF; // Lower 8 bits
    *upper_reg = after >> 8; // Upper 8 bits

    if(before + val > 0xFFFF) //Check for overflow
        *flags |= 1 << FLAG_CARRY;
    if((before & 0xFFF) + (val & 0xFFF) > 0xFFF) //Check for overflow on 11th bit
        *flags |= 1 << FLAG_HALF_CARRY;
    *flags &= ~(1 << FLAG_SUB);
}

void ADC(uint8_t *reg, uint8_t val, uint8_t *flags) {
    uint8_t before = *reg;
    uint8_t carry = *flags & (1 << FLAG_CARRY) ? 1 : 0;
    *reg += val + carry;

    if ((before & 0xF) + (val & 0xF) + carry > 0xF)
        *flags |= 1 << FLAG_HALF_CARRY;
    if (before + val + carry > 0xFF)
        *flags |= 1 << FLAG_CARRY;
    if (*reg == 0)
        *flags |= 1 << FLAG_ZERO;
    *flags &= ~(1 << FLAG_SUB);
}

void SBC(uint8_t *reg, uint8_t val, uint8_t *flags) {
    uint8_t before = *reg;
    uint8_t carry = *flags & (1 << FLAG_CARRY) ? 1 : 0;
    *reg -= val + carry;

    if ((before & 0xF) < (val & 0xF) + carry)
        *flags |= 1 << FLAG_HALF_CARRY;
    if (before < val + carry)
        *flags |= 1 << FLAG_CARRY;
    if (*reg == 0)
        *flags |= 1 << FLAG_ZERO;
    *flags |= 1 << FLAG_SUB;
}

void CP(uint8_t *reg, uint8_t val, uint8_t *flags){
    uint8_t before = *reg;
    
    if ((before & 0xF) < (val & 0xF)) // Check for half byte underflow
        *flags |= 1 << FLAG_HALF_CARRY; 
    if (before < val) // Check for byte underflow
        *flags |= 1 << FLAG_CARRY;
    if (before == val) // Check if result is 0
        *flags |= 1 << FLAG_ZERO;
    *flags |= 1 << FLAG_SUB;
}

void INC(uint8_t *reg, uint8_t *flags) {
    uint8_t before = *reg;
    *reg++;

    if (before & 0xF == 0xF)
        *flags |= 1 << FLAG_HALF_CARRY;
    if (*reg == 0)
        *flags |= 1 << FLAG_ZERO;
    *flags &= ~(1 << FLAG_SUB);
}

void DEC(uint8_t *reg, uint8_t *flags) {
    uint8_t before = *reg;
    *reg--;

    if (before & 0xF == 0x0)
        *flags |= 1 << FLAG_HALF_CARRY;
    if (*reg == 0)
        *flags |= 1 << FLAG_ZERO;
    *flags |= 1 << FLAG_SUB;
}

void AND(uint8_t *reg, uint8_t val, uint8_t *flags) {
    *reg &= val;
    *flags &= ~(1 << FLAG_SUB | 1 << FLAG_CARRY);
    *flags |= 1 << FLAG_HALF_CARRY;
    if(*reg == 0) 
        *flags |= 1 << FLAG_ZERO;
}

void OR(uint8_t *reg, uint8_t val, uint8_t *flags) {
    *reg |= val;
    *flags &= ~(1 << FLAG_CARRY | 1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (*reg == 0)
        *flags |= 1 << FLAG_ZERO;
}

void XOR(uint8_t *reg, uint8_t val, uint8_t *flags) {
    *reg ^= val;
    *flags &= ~(1 << FLAG_CARRY | 1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (*reg == 0)
        *flags |= 1 << FLAG_ZERO;
}

void CPL(uint8_t *reg, uint8_t *flags) {
    *reg = ~(*reg);
    *flags |= 1 << FLAG_HALF_CARRY | 1 << FLAG_SUB;
}

void BIT(uint8_t *reg, uint8_t bitPosition, uint8_t *flags) {
    if (*reg & (1 << bitPosition) == 0)
        *flags |= 1 << FLAG_ZERO;
    *flags |= 1 << FLAG_HALF_CARRY;
    *flags &= ~(1 << FLAG_SUB);
}

void RL(uint8_t *reg, uint8_t *flags, uint8_t isCircular, uint8_t isRegA) {
    uint8_t before = *reg;
    *reg = *reg << 1 | isCircular ? (before >> 7) : ((*flags >> FLAG_CARRY) & 1);
    *flags = (before >> 0x7) << FLAG_CARRY;

    *flags &= ~(1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (!isRegA && *reg == 0) 
        *flags |= 1 << FLAG_ZERO;
    else
        *flags &= ~(1 << FLAG_ZERO);
}

void RR(uint8_t *reg, uint8_t *flags, uint8_t isCircular, uint8_t isRegA) {
    uint8_t before = *reg;
    *reg = *reg >> 1 | isCircular ? (before & 0x80) : ((*flags >> FLAG_CARRY) << 7);
    *flags = (before & 1) << FLAG_CARRY;

    *flags &= ~(1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (!isRegA && *reg == 0) 
        *flags |= 1 << FLAG_ZERO;
    else
        *flags &= ~(1 << FLAG_ZERO);
}

void SLA(uint8_t *reg, uint8_t *flags) {
    *flags = (*reg >> 7) << FLAG_CARRY;
    *reg <<= 1;

    *flags &= ~(1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (*reg == 0) 
        *flags |= 1 << FLAG_ZERO;
    else
        *flags &= ~(1 << FLAG_ZERO);
}

void SRA(uint8_t *reg, uint8_t *flags) {
    *flags = (*reg & 1) << FLAG_CARRY;
    *reg = *reg >> 1 | (*reg & 0x80);

    *flags &= ~(1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (*reg == 0) 
        *flags |= 1 << FLAG_ZERO;
    else
        *flags &= ~(1 << FLAG_ZERO);
}

void SRL(uint8_t *reg, uint8_t *flags) {
    *flags = (*reg & 1) << FLAG_CARRY;
    *reg >>= 1;

    *flags &= ~(1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (*reg == 0) 
        *flags |= 1 << FLAG_ZERO;
    else
        *flags &= ~(1 << FLAG_ZERO);
}

void SWAP(uint8_t *reg, uint8_t *flags) {
    uint8_t upper = (*reg & 0b11110000) >> 4;
    *reg = *reg << 4 | upper;
    *flags &= ~(1 << FLAG_CARRY | 1 << FLAG_HALF_CARRY | 1 << FLAG_SUB);
    if (*reg == 0)
        *flags |= 1 << FLAG_ZERO;
}
