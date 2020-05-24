#pragma once
#include "MemoryInterface.hpp"
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
	FUT,
	INC, INX, INY,
	JMP, JSR,
	LDA, LDX, LDY, LSR,
	NOP,
	ORA,
	PHA, PHP, PLA, PLP,
	ROL, ROR, RTI, RTS,
	SBC, SEC, SED, SEI, STA, STX, STY,
	TAX, TAY, TSX, TXA, TXS, TYA
} op_code_t;

typedef enum AddressingMode
{
	Absolute, AbsoluteX, AbsoluteY,
	Accum_mode,
	IMM,
	Implied,
	IndexedIndirect,
	Indirect, IndirectIndexed,
	Relative,
	ZP, ZPX, ZPY,
} addressing_mode_t;


// instructionModes indicates the addressing mode for each instruction
extern AddressingMode instructionModes[256];

// instructionSizes indicates the size of each instruction in bytes
extern uint8_t instructionSizes[256]; 

// instructionCycles indicates the number of cycles used by each instruction,
// not including conditional cycles
extern uint8_t instructionCycle[256];

// instructionPageCycles indicates the number of cycles used by each
// instruction when a page is crossed
extern uint8_t instructionPageCycles[256];

// instructionNames indicates the name of each instruction
extern OpCode instructionNames[256];

typedef struct op_code_params{
	char16_t address;
	unsigned char operand;
	addressing_mode_t mode;
	uint8_t instructionSize;
} op_code_params_t;

// array of 56 lamda function opcodes that return void and take in whatever
extern std::function<void(CPU_6502*, op_code_params_t*)> opcode_to_func[256];
