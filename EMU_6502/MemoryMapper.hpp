#pragma once
#include <cstdint>

class MemoryMapper // interface for generating a memory map for an entire system, base class simply creates 64k of ram
{
private:
	char* addressSpace; // can't be directly modified

public:
	MemoryMapper()
	{
		addressSpace = new char[65536]; // children might not generate actual addressSpace in memory
	};
	
	MemoryMapper(uint32_t addrSpaceSize)
	{
		addressSpace = new char[addrSpaceSize]; // children might not generate actual addressSpace in memory
	};
	
	virtual ~MemoryMapper()
	{
		delete addressSpace;
	};
	
	virtual char read(uint16_t address) { return addressSpace[address]; };
	
	virtual char16_t read16(uint16_t address) {
		return ((addressSpace[address] << 8) | addressSpace[address + 1]);
	};
	
	virtual bool write(uint16_t address, char byte)
	{
		if (address > sizeof(addressSpace)) return false;
		addressSpace[address] = byte;
		return true;
	};

	virtual bool writeArray(uint16_t startAddress, char* bytes)
	{
		if (startAddress + sizeof(bytes) > sizeof(addressSpace)) return false; // would cause memory leak
		for(int i = 0; i<sizeof(bytes); i++)
		{
			addressSpace[startAddress + i] = bytes[i];
		}
		return true;
	};
};

