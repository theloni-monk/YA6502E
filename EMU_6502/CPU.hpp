#pragma once
#include "Operations.hpp"
#include "MemoryInterface.hpp"
#include <cstdint>

typedef enum Flag
{
	UNUSED,
	CARRY,
	ZERO,
	IRQ_DISABLE,
	DECIMAL_MODE,
	BRK_COMMAND,
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
	
	CPU_6502();

	~CPU_6502();

	unsigned char read(uint16_t address) override;

	void write(uint16_t address, char byte) override;

	// Derives opcode params based on opcode fetched using PC, returns via reference
	void fetch(op_code_t&, op_code_params_t&);

	// Executes op code based on pparameters passed to it as a pointer
	void execute(op_code_t, op_code_params_t);

	// allocate memory for registers and address space and initialize the program counter
	void reset(uint16_t);
};
