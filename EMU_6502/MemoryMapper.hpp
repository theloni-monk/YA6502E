#pragma once
#include <cstdint>
#include <cstdio>
#include <iostream>

class MemoryMapper // interface for generating a memory map for an entire system, base class simply creates 64k of ram
{
private:
	uint8_t* addressSpace; // can't be directly modified
	uint64_t addrSpaceSize;

public:
	MemoryMapper()
	{
		this->addressSpace = new uint8_t[65536](); // children might not generate actual addressSpace in memory
		this->addrSpaceSize = 65536;
	};
	
	MemoryMapper(uint64_t addrSpaceSize)
	{
		this->addressSpace = new uint8_t[addrSpaceSize](); // children might not generate actual addressSpace in memory
		this->addrSpaceSize = addrSpaceSize;
	};
	
	virtual ~MemoryMapper()
	{
		delete[] this->addressSpace;
	};
	
	virtual uint8_t read(uint16_t address) { return this->addressSpace[address];};
	
	virtual uint16_t read16(uint16_t address) {
		return ((this->addressSpace[address] << 8) | this->addressSpace[address + 1]);
	};
	
	virtual bool write(uint16_t address, char byte)
	{
		if (address > this->addrSpaceSize) return false;
		this->addressSpace[address] = byte;
		return true;
	};

	virtual bool writeArray(uint16_t startAddress,  uint8_t bytes[33], uint16_t programLength)
	{
		if (startAddress + programLength > addrSpaceSize) return false; // would cause memory leak
		
		for(int i = 0; i < programLength; i++) this->addressSpace[startAddress + i] = bytes[i];
		
		return true;
	};
};

