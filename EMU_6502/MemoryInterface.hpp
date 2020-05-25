#pragma once
#include <cstdint>

#include "MemoryMapper.h"

class MemoryInterface
{
public:
	MemoryMapper map;
	
	char read(uint16_t address) { return map.read(address); };
	void write(uint16_t address, char byte) { map.write(address, byte); };

	MemoryInterface(MemoryMapper m) { this->map = m; };
	//virtual ~MemoryInterface() { delete addressSpace; };
};
