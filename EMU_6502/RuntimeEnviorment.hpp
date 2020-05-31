#pragma once
#include <string>
#include "CPU.hpp"
#include "MemoryMapper.hpp"

class RuntimeEnviorment
{
public:
	CPU_6502* cpu;
	MemoryMapper* map;
	
	void loadProgram(std::string filePath, uint16_t programStart);
	
	RuntimeEnviorment();
	virtual ~RuntimeEnviorment();

	void init(std::string filePath);

	void visualize();

	void step();

	void run();

};
