#pragma once
#include "cpu.h"
#include "memory.h"

enum ConditionCode{
    Z, //zero flag is set
    NZ, //zero flag is cleared
    C, //carry flag is set
    NC, //carry flag is cleared
    NOC //No Condition
};

#define FALSE 0
#define TRUE 1

//Load operations
#define LD(reg, val) (reg) = (val)
#define LD_8_MEM(reg, address) (reg) = memoryRead(address) // Write cpu register from memory
#define LD_MEM_8(address, reg) memoryWrite((address), (reg)) // Write memory from cpu register
#define LDH_A_MEM() cpu.a = memoryRead(0xFF00 + cpuFetch())
#define LDH_MEM_A() memoryWrite((0xFF00 + cpuFetch()), cpu.a) 
#define LDH_A_C() cpu.a = memoryRead(cpu.c + 0xFF00)
#define LDH_C_A() memoryWrite((cpu.c + 0xFF00), cpu.a)
void LD_MEM_SP(void);
void LD_HL_SP(void);

//Arithmetic operations
void ADD8(uint8_t *reg, uint8_t val);
void SUB(uint8_t *reg, uint8_t val);
void ADD16(uint16_t val);
void ADC(uint8_t *reg, uint8_t val);
void SBC(uint8_t *reg, uint8_t val);
void CP(uint8_t *reg, uint8_t val);
void INC8(uint8_t *reg);
void DEC8(uint8_t *reg);

//Bitwise operations
void AND(uint8_t *reg, uint8_t val);
void OR(uint8_t *reg, uint8_t val);
void XOR(uint8_t *reg, uint8_t val);
void CPL(void);
void BIT(uint8_t *reg, uint8_t bitPosition);
void RL(uint8_t *reg, uint8_t isCircular, uint8_t isRegA);
void RR(uint8_t *reg, uint8_t isCircular, uint8_t isRegA);
void SLA(uint8_t *reg); //Shift Left Arithmetically
void SRA(uint8_t *reg); //Shift Right Arithmetically
void SRL(uint8_t *reg); //Shift Right Logically
void SWAP(uint8_t *reg);

//Subroutine operations
void JP(uint16_t address, enum ConditionCode cc);
void CALL(uint16_t address, enum ConditionCode cc);
void JR(int8_t offset, enum ConditionCode cc);
void RET(uint8_t enableInterrupt, enum ConditionCode cc);
void RST(uint8_t vec); //Value of vec will be determined by the opcode value

// Stack manipulation commands
void POP(uint16_t *reg);
void POP_AF(void);
void PUSH(uint16_t reg);
void PUSH_AF(void);
void ADD_SP(void);

//Carry flag operations
void CCF(void); //Invert carry flag
void SCF(void); // Set carry flag

//Interrupt operations
void DI(void); // Disable Interrupt
void EI(void); // Enable Interrupt
void HALT(void); //TODO

// Miscellaneous operations
void DAA(void);
void NOP(void);
void STOP(void);

// Static Functions
static uint8_t checkConditionCode(enum ConditionCode cc);
#define INC16(x) (x)++
#define DEC16(x) (x)--
#define RES(reg, bitPosition) (reg) &= ~(1 << (bitPosition))
#define SET(reg, bitPosition) (reg) |= 1 << (bitPosition)

#define const OPCODE_PREFIX 0xCB
