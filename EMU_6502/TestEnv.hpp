#pragma once
#include <string>

#include "CPU.hpp"

class TestEnv
{
public:
	MemoryMapper* map;
	CPU_6502* cpu;
	
	TestEnv();
	~TestEnv();

	void init(std::string filePath);

	void visualize();

	void step();

	void run();

private:
	void loadProgram(std::string filePath, uint16_t programStart);
};