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

	void visualize(uint8_t o, op_code_params_t params, int step);

	void step(int step);

	void run();

private:
	void loadProgram(std::string filePath, uint16_t programStart);
};