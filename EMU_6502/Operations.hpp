#pragma once
#include "CPU.hpp"
#include <functional>
class CPU_6502;

typedef enum op_code_t : uint8_t
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
};

typedef enum  addressing_mode_t : uint8_t
{
	UNUSED,
	Absolute, AbsoluteX, AbsoluteY,
	Accum_mode,
	Immediate,
	Implied,
	IndexedIndirect,
	Indirect, IndirectIndexed,
	Relative,
	ZP, ZPX, ZPY,
};


// instructionModes indicates the addressing mode for each instruction
extern addressing_mode_t instructionModes[256];

// instructionSizes indicates the size of each instruction in bytes
extern uint8_t instructionSizes[256]; 

// instructionCycles indicates the number of cycles used by each instruction,
// not including conditional cycles
extern uint8_t instructionCycle[256];

// instructionPageCycles indicates the number of cycles used by each
// instruction when a page is crossed
extern uint8_t instructionPageCycles[256];

// instructionNames indicates the name of each instruction
extern op_code_t instructionNames[256];

// used only by visualizer to print names, instructionNames is used as uint for faster lookup
extern char instructionChars[256][4];

typedef struct op_code_params{
	uint16_t address;
	uint8_t operand;
	addressing_mode_t mode;
	uint8_t instructionSize;
} op_code_params_t;

// array of 56 lamda function opcodes that return void and take in whatever
extern std::function<void(CPU_6502*, op_code_params_t*)> opcode_to_func[256];
