#pragma once
#include "CPU.h"

enum OpCode
{
	ADC = 0, AND, ASL,
	BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS,
	CLC, CLD, CLI, CLV, CMP, CPX, CPY,
	DEC, DEX, DEY,
	EOR,
	INC, INX, INY,
	JPM, JSR,
	LDA, LDX, LDY, LDR,
	NOP,
	ORA,
	PHA, PHP, PLA, PLP,
	ROL, ROR, RTI, RTS,
	SBC, SEC, SED, SEI, STA, STX, STY,
	TAX, TAY, TSX, TXA, TXS, TYA
};

struct op_code_params{
	char16_t address;
	unsigned char operand;
	AddressingMode mode;
};

// lamda function definition
typedef void (*operation)(CPU *c, op_code_params *o);

//array of 56 lamda function opcodes that return void and take in whatever
operation Ops[];
