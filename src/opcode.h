#pragma once
#include "cpu.h"

//Arithmetic operations
void ADD8(uint8_t *reg, uint8_t val, uint8_t *flags);
void SUB(uint8_t *reg, uint8_t val, uint8_t *flags);
void ADD16(uint8_t *upper_reg, uint8_t *lower_reg, uint16_t val, uint8_t *flags);
void ADC(uint8_t *reg, uint8_t val, uint8_t *flags);
void SBC(uint8_t *reg, uint8_t val, uint8_t *flags);
void CP(uint8_t *reg, uint8_t val, uint8_t *flags);
void INC(uint8_t *reg, uint8_t *flags);
void DEC(uint8_t *reg, uint8_t *flags);

//Bitwise operations
void AND(uint8_t *reg, uint8_t val, uint8_t *flags);
void OR(uint8_t *reg, uint8_t val, uint8_t *flags);
void XOR(uint8_t *reg, uint8_t val, uint8_t *flags);
void CPL(uint8_t *reg, uint8_t val, uint8_t *flags);
void BIT(uint8_t *reg, uint8_t bitPosition, uint8_t *flags);
void RL(uint8_t *reg, uint8_t *flags, uint8_t isCircular, uint8_t isRegA);
void RR(uint8_t *reg, uint8_t *flags, uint8_t isCircular, uint8_t isRegA);
void SLA(uint8_t *reg, uint8_t *flags); //Shift Left Arithmetically
void SRA(uint8_t *reg, uint8_t *flags); //Shift Right Arithmetically
void SRL(uint8_t *reg, uint8_t *flags); //Shift Right Logically
void SWAP(uint8_t *reg, uint8_t *flags);



#define LD(x, y) (x) = (y)

#define INC8(x, flags) {\
    if((x) & 0xF == 0xF)\
        (flags) |= 1 << FLAG_HALF_CARRY;\
    (x)++;\
    if((x) == 0)\
        (flags) |= 1 << FLAG_ZERO;\
    (flags) &= ~(1 << FLAG_SUB);\
}\

#define DEC8(x, flags) {\
    if((x) & 0xF == 0)\
        (flags) |= 1 << FLAG_HALF_CARRY;\
    (x)--;\
    if((x) == 0)\
        (flags) |= 1 << FLAG_ZERO;\
    (flags) |= 1 << FLAG_SUB;\
}\

// #define ADD8(x, y, flags) {\
//     if((x) & 0xF == 0xF)\
//         (flags) |= 1 << FLAG_HALF_CARRY;\
//     if((x) == 0xFF)\
//         (flags) |= 1 << FLAG_CARRY;\
//     (x) += (y);\
//     if((x) == 0)\
//         (flags) |= 1 << FLAG_ZERO;\
//     (flags) &= ~(1 << FLAG_SUB);\
// }\

// #define SUB8(x, y, flags) {\
//     if(((x) & 0xF) < ((y) & 0xF))\
//         (flags) |= 1 << FLAG_HALF_CARRY;\
//     if((x) < (y))\
//         (flags) |= 1 << FLAG_CARRY;\
//     (x) -= (y);\
//     if((x) == 0)\
//         (flags) |= 1 << FLAG_ZERO;\
//     (flags) |= 1 << FLAG_SUB;\
// }\

// #define ADD16(x, y, flags) {\
//     if((x) == 0xFFF)\
//         (flags) |= 1 << FLAG_HALF_CARRY;\
//     if((x) == 0xFFFF)\
//         (flags) |= 1 << FLAG_CARRY;\
//     (x) += (y);\
//     (flags) &= ~(1 << FLAG_SUB);\
// }\

// #define ADC8(x, y, flags) {\
//     if(((x) & 0xF) + ((y) & 0xF) + (flags & FLAG_CARRY ? 1 : 0) > 0xF)\
//         (flags) |= 1 << FLAG_HALF_CARRY;\
//     if((x) + (y) + (flags & FLAG_CARRY ? 1 : 0) > 0xFF)\
//         (flags) |= 1 << FLAG_CARRY;\
//     (x) += (y) + (flags & FLAG_CARRY ? 1 : 0);\
//     (flags) &= ~(1 << FLAG_SUB);\
// }\

#define INC16(x) (x)++
#define DEC16(x) (x)--
#define RES(reg, bitPosition) (reg) &= ~(1 << (bitPosition))
#define SET(reg, bitPosition) (reg) |= 1 << (bitPosition)
// #define AND(reg, val, flags) {
//     (reg) &= (val);\
//     *(flags) &= ~()
// }\
// #define ADD()