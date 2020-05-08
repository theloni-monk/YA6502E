﻿#include "CPU.hpp"
#include "Operations.hpp"

//TODO: implement reads and writes through memoryinterface

CPU_6502::CPU_6502(unsigned char* mem_ptr):MemoryInterface(mem_ptr)
{
	this->regs = new unsigned char[5];
	this->regs[STACK] = 0x0100;
	this->Pc = 0x0000;
}
CPU_6502::~CPU_6502()
{
	delete this->addressSpace;
	delete this->regs;
	//delete this;
}


//TODO for memory mapped io
unsigned char CPU_6502::read(uint16_t address)
{
	return this->addressSpace[address];
}
//TODO: for memory mapped io
void CPU_6502::write(uint16_t address, char byte)
{
	this->addressSpace[address] = byte;
}


// reset regs and addressSpace and reinitialize the program counter
void CPU_6502::reset(uint16_t PC_start)
{
	delete this->regs;
	this->regs = new unsigned char[5];
	this->regs[STACK] = 0x0100;
	
	delete this->addressSpace;
	this->addressSpace = new unsigned char[65532];
	
	this->Pc = PC_start;
}

// helper struct factory
op_code_params_t makeParams(unsigned char operand, char16_t addr, addressing_mode_t addrMode)
{
	op_code_params o;
	o.address = addr;
	o.operand = operand;
	o.mode = addrMode;
	return o;
}
// Derives opcode params based on opcode fetched using PC, returns via reference
void CPU_6502::fetch(op_code_t &op, op_code_params_t &params)
{
	op_code_t opcode = instructionNames[this->addressSpace[this->Pc]];
	
	if(instructionNames[opcode] == FUT)
	{
		throw "Exception! Unimplemented OpCode";
	}

	op_code_params_t op_params;
	
	addressing_mode_t mode = instructionModes[opcode];

	uint16_t address;
	uint8_t operand;
	 
	switch(mode) // addressign modes modify how the fetch operation aquires the operand of the instruction 
	{
		case Absolute:
			//piece together 16 bit address from 2 bytes
			//following op code
		{
			//curly braces to allow for variable declaration
			//inside of case statement
			uint8_t lowerByte = this->addressSpace[this->Pc + 1];
			uint8_t upperByte = this->addressSpace[this->Pc + 2];
			address = (upperByte << 8) | lowerByte;
			op_params = makeParams(0, address, mode);
		}
		break;
		case AbsoluteX:
		{
			uint8_t lowerByte = this->addressSpace[this->Pc + 1];
			uint8_t upperByte = this->addressSpace[this->Pc + 2];
			address = (upperByte << 8) | lowerByte;
			uint16_t xVal = this->regs[IND_X];
			op_params = makeParams(0, address + xVal, mode);
		}
		break;
		case AbsoluteY:
		{
			uint8_t lowerByte = this->addressSpace[this->Pc + 1];
			uint8_t upperByte = this->addressSpace[this->Pc + 2];
			address = (upperByte << 8) | lowerByte;
			uint16_t yVal =this->regs[IND_Y];
			op_params = makeParams(0, address + yVal, mode);
		}
		break;
		case Accum:
			address = 0; //ADDRESS IS NOT APPLICABLE IN THIS MODE
			operand = this->regs[ACCUM];
			op_params = makeParams(operand, address, mode);
			break;
		case IMM:
			address = this->Pc + 1;
			operand = this->addressSpace[address];
			op_params = makeParams(operand, address, mode);
			break;
		case Implied:
			address = this->Pc;
			op_params = makeParams(0, address, mode);
			break;
		case IndexedIndirect:
		{
			uint16_t immediateVal = this->addressSpace[this->Pc + 1];
			uint16_t xVal = this->regs[IND_X];
			uint8_t lowerByte = this->addressSpace[immediateVal + xVal];
			uint8_t upperByte = this->addressSpace[immediateVal + xVal + 1];
			address = (upperByte << 8) | lowerByte;
			operand = this->addressSpace[address];
			op_params = makeParams(operand, address, mode);
		}
		break;
		case Indirect:
		{
			uint8_t lowerByte = this->addressSpace[this->Pc + 1];
			uint8_t upperByte = this->addressSpace[this->Pc + 2];
			address = (upperByte << 8) | lowerByte;
			uint8_t l = this->addressSpace[address];
			uint8_t u = this->addressSpace[address + 1];
			uint16_t finalAddress = (u << 8) | l;
			op_params = makeParams(0, finalAddress, mode);
		}
		break;
		case IndirectIndexed:
		{
			uint16_t immediateVal = this->addressSpace[this->Pc + 1];
			uint8_t lowerByte = this->addressSpace[immediateVal];
			uint8_t upperByte = this->addressSpace[immediateVal + 1];
			uint16_t yVal = this->regs[IND_Y];
			address = ((upperByte << 8) | lowerByte) + yVal;
			operand = this->addressSpace[address];
			op_params = makeParams(operand, address, mode);
		}
		break;
		case Relative:
			//for branching within +-128 
		{
			int16_t offset = this->addressSpace[this->Pc + 1];
			address = this->Pc + 2 + offset;
			op_params = makeParams(0, address, mode);
		}
		break;
		case ZP:
			address = 0x00FF & this->addressSpace[this->Pc + 1];
			operand = this->addressSpace[address];
			op_params = makeParams(operand, address, mode);
			break;
		case ZPX:
		{
			address = 0x00FF & this->addressSpace[this->Pc + 1];
			int8_t xVal = this->regs[IND_X];
			address += xVal;
			operand = this->addressSpace[address];
			op_params = makeParams(operand, address, mode);
		}
		break;
		case ZPY:
		{
			address = 0x00FF & this->addressSpace[this->Pc + 1];
			int8_t yVal = this->regs[IND_Y];
			address += yVal;
			operand = this->addressSpace[address];
			op_params = makeParams(operand, address, mode);
		}
		break;
	}
	
	op_params.instructionSize = instructionSizes[opcode];

	// change values passed by reference
	op = opcode;
	params = op_params;
}

// Executes op code based on pparameters passed to it as a pointer
void CPU_6502::execute(op_code_t op, op_code_params_t params)
{
	opcode_to_func[op](this, params);
	this->Pc += instructionSizes[params.instructionSize];// go to next opcode
}


