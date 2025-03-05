#include "MemoryScanner.h"

HANDLE MemoryScanner::getProcessHandle(DWORD pid)
{
	HANDLE result = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (result == NULL)
	{
		std::cout << GetLastError();
		return NULL;
	}
	else { return result; }
}

std::vector<MemoryScanner::MemoryBlock> MemoryScanner::getMemoryInformation(HANDLE hProcess, unsigned char* baseAddress)
{
	MEMORY_BASIC_INFORMATION memoryInfo;
	std::vector<MemoryScanner::MemoryBlock> result;
	while (VirtualQueryEx(hProcess, baseAddress, &memoryInfo, sizeof(memoryInfo)) != 0)
	{
		if ((memoryInfo.Protect & PAGE_READWRITE) && memoryInfo.State == MEM_COMMIT) { result.push_back({ hProcess, &memoryInfo }); }
		baseAddress = (unsigned char*)memoryInfo.BaseAddress + memoryInfo.RegionSize;
	}
	return result;
}

boolean MemoryScanner::updateMemoryBlock(MemoryBlock &block, SIZE_T &bytesRead)
{
	if(ReadProcessMemory(block.process, block.baseAddress, block.buffer, block.size, &bytesRead) != 0)
	{
		return true;
	}
	return false;
}

std::vector<MemoryScanner::Match> MemoryScanner::MemorySearch(std::vector<MemoryScanner::MemoryBlock> blocks, void* valueToSearch, int sizeOfValue, SIZE_T &bytesRead)
{
	std::vector<MemoryScanner::Match> result;
	int valueRead = 0;
	for (auto& block : blocks)
	{
		if (MemoryScanner::updateMemoryBlock(block, bytesRead))
		{
			for (int i = 0; i < bytesRead; i++)
			{
				switch (sizeOfValue)
				{
				case 1:
					if (*(char*)valueToSearch == (char)block.buffer[i]) { std::cout << (char)block.buffer[i]; result.push_back(MemoryScanner::Match(i, sizeOfValue, block)); }
					break;

				case 4:
					
					unsigned char temp[4] = { block.buffer[i], block.buffer[i + 1], block.buffer[i + 2], block.buffer[i + 3] };
					std::memcpy(&valueRead, temp, 4);
					if (*(int*)valueToSearch == valueRead) { result.push_back(MemoryScanner::Match(i, sizeOfValue, block)); }
					i += 3;
					break;
				}
			}
		}
	}
	return result;
}
std::vector<MemoryScanner::Match> MemoryScanner::MemorySearch(std::vector<MemoryScanner::Match> matches, void* valueToSearch, int sizeOfValue, SIZE_T& bytesRead)
{
	std::vector<MemoryScanner::Match> result;
	int valueRead = 0;
	for (auto& match : matches)
	{
		if (updateMemoryBlock(match.block, bytesRead))
		{
			switch (sizeOfValue)
			{
			case 1:
				if (*(char*)valueToSearch == (char)match.block.buffer[match.offset]) { result.push_back(MemoryScanner::Match(match.offset, sizeOfValue, match.block)); }
				break;

			case 4:
				unsigned char temp[4] = { match.block.buffer[match.offset], match.block.buffer[match.offset + 1], match.block.buffer[match.offset + 2], match.block.buffer[match.offset + 3] };
				std::memcpy(&valueRead, temp, 4);
				if (*(int*)valueToSearch == valueRead) { result.push_back(MemoryScanner::Match(match.offset, sizeOfValue, match.block)); }
				break;
			}
		}
	}
	return result;
}