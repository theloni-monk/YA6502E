#pragma once


#include "TestEnv.hpp"
#include "inttypes.h"
#include <stdlib.h>
#include <iostream>

#define DEBUG false

TestEnv::TestEnv()
{
	init("");
}

TestEnv::~TestEnv()
{
	delete map;
	delete cpu;
}


void TestEnv::init(std::string filePath)
{
	this->map = new MemoryMapper();
	cpu = new CPU_6502(map);
	loadProgram("",0x00);
}

void TestEnv::step(int step)
{
	uint8_t op;
	op_code_params_t info;

	cpu->fetch(op, info);

	cpu->execute(op, info);

	visualize(op, info, step);
}

void printDebug(CPU_6502* cpu, uint8_t o, op_code_params_t params)
{
	printf("\n\nProgram Counter: %#x", (cpu->getPc())); // vis called after program counter already incremented by instruction
	printf("\nOPCODE: %s", instructionChars[o]);
	printf("\n\tInstruction Mode: %u", params.mode);
	printf("\n\tAddress: %#x", params.address);
	printf("\n\tOperand: %#x", params.operand);
	printf("\n\tInstruction Size: %u", params.instructionSize);

	printf("\nREGISTERS: ");
	// printing status as a binary string
	int status = (int)cpu->getReg(STATUS);
	char statusBuffer[8];
	_itoa_s(status, statusBuffer, 2);

	printf("\n\tStatus: 0b%s", statusBuffer);
	printf("\n\tAccumulator: %u", cpu->getReg(ACCUM));
	printf("\n\tIndex X: %u", cpu->getReg(IND_X));
	printf("\n\tIndex Y: %u", cpu->getReg(IND_Y));
	printf("\n\tSTACK: %u", cpu->getReg(STACK));
}

void TestEnv::visualize(uint8_t o, op_code_params_t params, int step)
{
	if(DEBUG) printDebug(cpu, o, params);
	if(step%3 == 0) printf("\n\nIndex X: %u", cpu->getReg(IND_X));
	
	
}

const int steps = 1000;
void TestEnv::run()
{
	for(int i = 0; i< steps; i++)
	{
		step(i);
		
	}
}



void TestEnv::loadProgram(std::string filePath, uint16_t programStart)
{
	uint8_t fibonacciProgram[33]{
	0xA2, 0x01, 0x8E, 0x00, 0x00, 0x38, 0xA0, 0x07,
	0x98, 0xE9, 0x03, 0xA8, 0x18, 0xA9, 0x02, 0x8D,
	0x01, 0x00, 0xAE, 0x01, 0x00, 0x6D, 0x00, 0x00,
	0x8D, 0x01, 0x00, 0x8E, 0x00, 0x00, 0x88, 0xD0, 0xF1
	};
	
	uint8_t basicInc[7]
	{
		0xA2, 0x02, 0xE8, 0x38, 0xB0, 0xFC
	};
	
	if(!map->writeArray(programStart, basicInc, 6)) _ASSERT(0);
}
