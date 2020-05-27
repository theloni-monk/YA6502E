#pragma once

#include "MemoryInterface.hpp"
#include "Operations.hpp"
#include <cstdint>

typedef enum Flag
{
	CARRY,
	ZERO,
	IRQ_DISABLE,
	DECIMAL_MODE,
	BRK_COMMAND,
	UNUSED,
	OVRFLW, //OVERFLOW is a reserved word in c++
	NEGATIVE
} flag_t;

typedef enum REG
{
	STATUS, // flags 
	STACK, // stack pointer
	ACCUM, // accumulator

	// index/general registers
	IND_X,
	IND_Y
}reg_t;

typedef struct op_code_params op_code_params_t;

typedef enum OpCode op_code_t;

class CPU_6502: public MemoryInterface
{
private:
	uint8_t* regs; // registers
	uint16_t Pc; // program counter
	uint64_t cycles; // TODO: implement cycle counting
	
public:
	CPU_6502();
	
	CPU_6502(MemoryMapper* m);

	~CPU_6502();

	uint8_t getReg(reg_t reg) { return this->regs[reg]; }
	uint8_t* getRegs() { return this->regs; }

	void setReg(reg_t reg, uint8_t byte) { this->regs[reg] = byte; }

	
	uint16_t getPc() { return this->Pc; }

	void setPc(uint16_t pc) { this->Pc = pc; }

	
	uint64_t getCycles() { return this->cycles; }

	void setCycles(uint64_t cycles) { this->cycles = cycles; }

	
	// Derives opcode params based on opcode fetched using PC, returns via reference
	void fetch(op_code_t&, op_code_params_t&);

	// Executes op code based on pparameters passed to it as a pointer
	void execute(op_code_t, op_code_params_t);

	// allocate memory for registers and address space and initialize the program counter
	void reset(uint16_t);
};
