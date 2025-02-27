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
		PVOID baseAddress;
		SIZE_T size;
		unsigned char* buffer;

		MemoryBlock(HANDLE hProcess, MEMORY_BASIC_INFORMATION* memoryInfo) :
			process(hProcess), baseAddress(memoryInfo->BaseAddress), size(memoryInfo->RegionSize), buffer((unsigned char*)malloc(memoryInfo->RegionSize)) {
		}
	};

	struct Match
	{
		PVOID address;
		LPVOID value, searchValue;

		Match(PVOID addr, LPVOID val, LPVOID searchVal) :
			address(addr), value(val), searchValue(searchVal) {
		}
	};


public:
	HANDLE getProcessHandle(DWORD pid);
	std::vector<MemoryScanner::MemoryBlock> getMemoryInformation(HANDLE hProcess, unsigned char* baseAddress);
	boolean updateMemoryBlock(MemoryBlock &block, SIZE_T &bytesRead);
	void performOperations(); //to DELETE, just a placeholder for testing
};

