#pragma once
#include <cstdint>

class MemoryInterface
{
public:
	unsigned char* addressSpace;
	
	virtual unsigned char read(uint16_t address);
	virtual void write(uint16_t address, char byte);

	virtual ~MemoryInterface; //FIXME
};
