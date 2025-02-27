#include "MemoryScanner.h"

HANDLE MemoryScanner::getProcessHandle(DWORD pid)
{
	HANDLE result = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (result == NULL)
	{
		std::cout << GetLastError();
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

void MemoryScanner::performOperations()
{
	DWORD pid; //15144   - sizeof(const char*)
	HANDLE process;
	unsigned char* baseAddr = 0;
	SIZE_T bytesRead;

	std::cout << "Process ID: ";
	std::cin >> pid;
	process = getProcessHandle(pid);


	std::vector<MemoryScanner::MemoryBlock> memInfo = MemoryScanner::getMemoryInformation(process, baseAddr);

	int matchCount = 0;
	for (auto& block : memInfo) //Very barbone search, no iterations possible, has to be expanded in own function
	{
		if (updateMemoryBlock(block, bytesRead))
		{
			std::cout << "Addr: " << block.baseAddress << "\n" << "Bytes read: " << bytesRead << "\n";
			for (int i = 0; i < bytesRead; i++)
			{
				switch(sizeof(int))
				{
				case 1:
					//std::cout << (char)(block.buffer[i]);
					break;
				
				case 4:
					//std::cout << (int)(block.buffer[i]);
					if (95 == (int)block.buffer[i]) { matchCount++; }
					i += 3;
					break;
				}
				//std::cout << "\n";
			}
		}
	}
	std::cout << "Match count: " << matchCount;
}