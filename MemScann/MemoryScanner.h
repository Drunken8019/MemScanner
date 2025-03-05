#pragma once
#include <iostream>
#include <windows.h>
#include <vector>

class MemoryScanner
{
public:
	struct MemoryBlock
	{
		HANDLE process;
		unsigned char* baseAddress;
		SIZE_T size;
		unsigned char* buffer;

		MemoryBlock(HANDLE hProcess, MEMORY_BASIC_INFORMATION* memoryInfo) :
			process(hProcess), baseAddress((unsigned char*)memoryInfo->BaseAddress), size(memoryInfo->RegionSize), buffer((unsigned char*)malloc(memoryInfo->RegionSize)) {
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

public:
	HANDLE getProcessHandle(DWORD pid);
	std::vector<MemoryScanner::MemoryBlock> getMemoryInformation(HANDLE hProcess, unsigned char* baseAddress);
	boolean updateMemoryBlock(MemoryBlock& block, SIZE_T& bytesRead);
	std::vector<MemoryScanner::Match> MemorySearch(std::vector<MemoryScanner::MemoryBlock> blocks, void* valueToSearch, int sizeOfValue, SIZE_T& bytesRead);
	std::vector<MemoryScanner::Match> MemorySearch(std::vector<MemoryScanner::Match> matches, void* valueToSearch, int sizeOfValue, SIZE_T& bytesRead);
};

