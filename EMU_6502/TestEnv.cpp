#pragma once


#include "TestEnv.hpp"

#include <iostream>
//TODO: writeme
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

void TestEnv::step()
{
	op_code_t op;
	op_code_params_t info;

	cpu->fetch(op, info);

	cpu->execute(op, info);
}

void TestEnv::visualize()
{
	std::cout << (cpu->getRegs()) << '\n';
}

const int steps = 20;
void TestEnv::run()
{
	for(int i = 0; i< steps; i++)
	{
		step();
		visualize();
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
	
	if(!map->writeArray(programStart, fibonacciProgram, 33)) _ASSERT(0);
}
