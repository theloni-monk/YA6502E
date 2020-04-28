#pragma once
#include "CPU.hpp"
#include <functional>
class CPU_6502;

typedef enum OpCode
{
	ADC, AND, ASL,
	BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS,
	CLC, CLD, CLI, CLV, CMP, CPX, CPY,
	DEC, DEX, DEY,
	EOR,
	INC, INX, INY,
	JMP, JSR,
	LDA, LDX, LDY, LSR,
	ORA,
	PHA, PHP, PLA, PLP,
	ROL, ROR, RTI, RTS,
	SBC, SEC, SED, SEI, STA, STX, STY,
	TAX, TAY, TSX, TXA, TXS, TYA,
	NOP,
	FUT
} op_code_t;

typedef enum AddressingMode
{
	Absolute, AbsoluteX, AbsoluteY,
	Accum,
	IMM,
	Implied,
	IndexedIndirect,
	Indirect, IndirectIndexed,
	Relative,
	ZP, ZPX, ZPY,
} addressing_mode_t;


// instructionModes indicates the addressing mode for each instruction
AddressingMode instructionModes[256] = {
	Implied, IndexedIndirect, Implied, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Accum, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPX, ZPX, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteX, AbsoluteX,
	Absolute, IndexedIndirect, Implied, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Accum, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPX, ZPX, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteX, AbsoluteX,
	Implied, IndexedIndirect, Implied, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Accum, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPX, ZPX, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteX, AbsoluteX,
	Implied, IndexedIndirect, Implied, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Accum, IMM, Indirect, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPX, ZPX, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteX, AbsoluteX,
	IMM, IndexedIndirect, IMM, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Implied, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPY, ZPY, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteY, AbsoluteY,
	IMM, IndexedIndirect, IMM, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Implied, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPY, ZPY, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteY, AbsoluteY,
	IMM, IndexedIndirect, IMM, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Implied, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPX, ZPX, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteX, AbsoluteX,
	IMM, IndexedIndirect, IMM, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Implied, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPX, ZPX, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteX, AbsoluteX,
};


// instructionSizes indicates the size of each instruction in bytes
uint8_t instructionSizes[256] = {
	1, 2, 0, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0,
	3, 2, 0, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0,
	1, 2, 0, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0,
	1, 2, 0, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 0, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 0, 3, 0, 0,
	2, 2, 2, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0
};

// instructionCycles indicates the number of cycles used by each instruction,
// not including conditional cycles
uint8_t instructionCycle[256] = {
	7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
	2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
};

// instructionPageCycles indicates the number of cycles used by each
// instruction when a page is crossed
uint8_t instructionPageCycles[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
};

// instructionNames indicates the name of each instruction
OpCode instructionNames[256] = {
	//FUT represents unimplemented op codes
	BRK, ORA, FUT, FUT, FUT, ORA, ASL, FUT,
	PHP, ORA, ASL, FUT, FUT, ORA, ASL, FUT,
	BPL, ORA, FUT, FUT, FUT, ORA, ASL, FUT,
	CLC, ORA, FUT, FUT, FUT, ORA, ASL, FUT,
	JSR, AND, FUT, FUT, BIT, AND, ROL, FUT,
	PLP, AND, ROL, FUT, BIT, AND, ROL, FUT,
	BMI, AND, FUT, FUT, FUT, AND, ROL, FUT,
	SEC, AND, FUT, FUT, FUT, AND, ROL, FUT,
	RTI, EOR, FUT, FUT, FUT, EOR, LSR, FUT,
	PHA, EOR, LSR, FUT, JMP, EOR, LSR, FUT,
	BVC, EOR, FUT, FUT, FUT, EOR, LSR, FUT,
	CLI, EOR, FUT, FUT, FUT, EOR, LSR, FUT,
	RTS, ADC, FUT, FUT, FUT, ADC, ROR, FUT,
	PLA, ADC, ROR, FUT, JMP, ADC, ROR, FUT,
	BVS, ADC, FUT, FUT, FUT, ADC, ROR, FUT,
	SEI, ADC, FUT, FUT, FUT, ADC, ROR, FUT,
	FUT, STA, FUT, FUT, STY, STA, STX, FUT,
	DEY, FUT, TXA, FUT, STY, STA, STX, FUT,
	BCC, STA, FUT, FUT, STY, STA, STX, FUT,
	TYA, STA, TXS, FUT, FUT, STA, FUT, FUT,
	LDY, LDA, LDX, FUT, LDY, LDA, LDX, FUT,
	TAY, LDA, TAX, FUT, LDY, LDA, LDX, FUT,
	BCS, LDA, FUT, FUT, LDY, LDA, LDX, FUT,
	CLV, LDA, TSX, FUT, LDY, LDA, LDX, FUT,
	CPY, CMP, FUT, FUT, CPY, CMP, DEC, FUT,
	INY, CMP, DEX, FUT, CPY, CMP, DEC, FUT,
	BNE, CMP, FUT, FUT, FUT, CMP, DEC, FUT,
	CLD, CMP, FUT, FUT, FUT, CMP, DEC, FUT,
	CPX, SBC, FUT, FUT, CPX, SBC, INC, FUT,
	INX, SBC, NOP, FUT, CPX, SBC, INC, FUT,
	BEQ, SBC, FUT, FUT, FUT, SBC, INC, FUT,
	SED, SBC, FUT, FUT, FUT, SBC, INC, FUT
};

typedef struct op_code_params{
	char16_t address;
	unsigned char operand;
	addressing_mode_t mode;
	uint8_t instructionSize;
} op_code_params_t;

// array of 56 lamda function opcodes that return void and take in whatever
extern std::function<void(CPU_6502*, op_code_params_t)> opcode_to_func[256];
