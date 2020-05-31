// EMU_6502.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma once

#include "TestEnv.hpp"
#include <iostream>
int main()
{
    std::cout << "Fibonacci!\n";
	TestEnv* t = new TestEnv();

	t->run();
	
	delete t;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
