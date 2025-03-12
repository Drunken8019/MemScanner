#pragma once
#include <iostream>
#include <windows.h>
#include <vector>

class MemoryScanner
{
public:
	HANDLE process;
	struct MemoryBlock
	{
		unsigned char* baseAddress;
		SIZE_T size;
		unsigned char* buffer;

		MemoryBlock(MEMORY_BASIC_INFORMATION* memoryInfo) :
			baseAddress((unsigned char*)memoryInfo->BaseAddress), size(memoryInfo->RegionSize), buffer((unsigned char*)malloc(memoryInfo->RegionSize)) {
		}
	};

	struct Match
	{
		SIZE_T offset;
		SIZE_T valueSize;
		MemoryBlock block;

		Match(SIZE_T off, SIZE_T valSize, MemoryBlock b) :
			offset(off), valueSize(valSize), block(b) {
		}
	};

	MemoryScanner()
	{

	}

	MemoryScanner(DWORD pid) :
		process(getProcessHandle(pid)), blocks(getMemoryInformation(0)) {
	}

	std::vector<MemoryScanner::Match> matches;
	std::vector<MemoryScanner::MemoryBlock> blocks;

	HANDLE getProcessHandle(DWORD pid);
	std::vector<MemoryScanner::MemoryBlock> getMemoryInformation(unsigned char* baseAddress);
	boolean updateMemoryBlock(MemoryBlock& block, SIZE_T& bytesRead);
	void initMemorySearch(void* valueToSearch, int sizeOfValue, SIZE_T& bytesRead);
	void MemorySearch(void* valueToSearch, int sizeOfValue, SIZE_T& bytesRead);
	int search(void* valueToSearch, int sizeOfValue, SIZE_T& bytesRead);
	boolean writeToAllMatches(void* valueToSearch, int sizeOfValue, SIZE_T& bytesRead);
};

