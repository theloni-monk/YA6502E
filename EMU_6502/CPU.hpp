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
public:

	unsigned char* regs; // registers
	char16_t Pc; // program counter
	uint64_t cycles; // TODO: implement cycle counting
	
	CPU_6502(unsigned char* mem_ptr);

	~CPU_6502();


	// Derives opcode params based on opcode fetched using PC, returns via reference
	void fetch(op_code_t&, op_code_params_t&);

	// Executes op code based on pparameters passed to it as a pointer
	void execute(op_code_t, op_code_params_t);

	// allocate memory for registers and address space and initialize the program counter
	void reset(uint16_t);
};
