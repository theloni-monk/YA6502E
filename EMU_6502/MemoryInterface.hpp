#pragma once
#include <cstdint>

class MemoryInterface
{
public:
	unsigned char* addressSpace;
	
	virtual unsigned char read(uint16_t address) { return addressSpace[address]; };
	virtual void write(uint16_t address, char byte) { addressSpace[address] = byte; };

	MemoryInterface(unsigned char* mem_ptr) { this->addressSpace = mem_ptr; };
	//virtual ~MemoryInterface() { delete addressSpace; };
};
