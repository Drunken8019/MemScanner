#pragma once
#include <iostream>
#include <windows.h>
#include <vector>

class MemoryScanner
{
	HANDLE process;
protected:
	struct MemoryBlock
	{
		HANDLE process;
		unsigned char* baseAddress;
		SIZE_T size;
		char* buffer;

		MemoryBlock(HANDLE hProcess, MEMORY_BASIC_INFORMATION* memoryInfo) :
			process(hProcess), baseAddress((unsigned char*)memoryInfo->BaseAddress), size(memoryInfo->RegionSize), buffer((char*)malloc(memoryInfo->RegionSize)) {
		}
	};


public:
	HANDLE getProcessHandle(DWORD pid);
	std::vector<MemoryScanner::MemoryBlock> getMemoryInformation(HANDLE hProcess, unsigned char* baseAddress);
	void performOperations(); //to DELETE, just a placeholder for testing
};

