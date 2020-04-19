#pragma once
#include "Operations.h"

enum AddressingMode
{
	Accum,
	IMM,
	Absolute,
	ZP, 
	Implied,
	Relative,
	Indirect
};

enum Flag
{
	CARRY,
	ZERO,
	IRQ_DISABLE,
	DECIMAL_MODE,
	BRK_COMMAND,
	OVRFLW, //OVERFLOW is a reserved word in c++
	NEGATIVE
};

enum REG
{
	STATUS, // flags 
	STACK, // stack pointer
	ACCUM, // accumulator

	// index/general registers
	IND_X,
	IND_Y
};

class CPU
{
public:
	
	unsigned char *regs[5]; // registers
	char16_t Pc; // program counter
	
	char *addressSpace[65536]; // memory

	static void fetch(){}

	static void execute(OpCode){}

	// allocate memory for registers and address space and initialize the program counter
	static void initialize(){}
	

};
