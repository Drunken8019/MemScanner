// MemScann.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "MemoryScanner.h"
#include <iostream>

int main()
{
    MemoryScanner m1;

	DWORD pid;
	HANDLE process;
	unsigned char* baseAddr = 0;
	SIZE_T bytesRead;
	int d = 0;
	int x = 0;
	std::cout << "Process ID: ";
	std::cin >> pid;
	process = m1.getProcessHandle(pid);


	std::vector<MemoryScanner::MemoryBlock> memInfo = m1.getMemoryInformation(process, baseAddr);
	int matchCount = 0;
	std::vector<MemoryScanner::Match> s1;
	std::cout << "Enter value: ";
	std::cin >> d;
	s1 = m1.MemorySearch(memInfo, &d, 4, bytesRead);
	for (auto& a : s1)
	{
		//WriteProcessMemory(a.block.process, a.block.baseAddress + a.offset, &x, sizeof(x), NULL);
		matchCount++;
	}
	std::cout << "Match count: " << matchCount << "\n";
	matchCount = 0;
	while (d != -1)
	{
		std::cout << "Enter next value: ";
		std::cin >> d;
		if (d == -2)
		{
			for (auto& a : s1)
			{
				std::cout << (unsigned char)a.block.baseAddress + a.offset << "\n";
			}
		}
		else if (d == -3)
		{
			std::cout << "Value to write: ";
			std::cin >> x;
			for (auto& a : s1)
			{
				WriteProcessMemory(a.block.process, a.block.baseAddress + a.offset, &x, sizeof(x), NULL);
			}
		}
		else if (d == -4)
		{
			
			for (auto& a : s1)
			{
				m1.memDump(a.block.process, a.block.baseAddress, a.offset, sizeof(int));
			}
		}
		else if (d != -1)
		{
			s1 = m1.MemorySearch(s1, &d, 4, bytesRead);
			for (auto& a : s1)
			{
				//WriteProcessMemory(a.block.process, a.block.baseAddress + a.offset, &x, sizeof(x), NULL);
				matchCount++;
			}
			std::cout << "Match count: " << matchCount << "\n";
			matchCount = 0;
		}
	}
}
