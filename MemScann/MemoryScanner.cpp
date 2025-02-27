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

void MemoryScanner::performOperations()
{
	DWORD pid; //15144   - sizeof(const char*) 
	//testcomment
	HANDLE process;
	unsigned char* baseAddr = 0;
	int searchInt = 199;
	SIZE_T* bytesRead{};
	std::cout << "Process ID: ";
	std::cin >> pid;
	process = getProcessHandle(pid);
	double fullSize = 0;
	std::vector<MemoryScanner::MemoryBlock> memInfo = MemoryScanner::getMemoryInformation(process, baseAddr);
	for (auto& block : memInfo)
	{

		/*if (WriteProcessMemory(block.process, block.baseAddress, yuhu, sizeof(yuhu), NULL) != NULL)
		{
			std::cout << "wrote\n";
		}
		*/

		if (ReadProcessMemory(block.process, block.baseAddress, block.buffer, block.size, bytesRead) != 0)
		{

			for (int i = 0; i < block.size - sizeof(searchInt); i++)
			{
				//if (memcmp(&block.buffer[i], &searchInt, sizeof(searchInt)) == 0)
				if ((int)block.buffer[i] == searchInt)
				{
					std::cout << &block.baseAddress + i << ": " << (int)block.buffer[i];
					std::cout << "\n";
				}
				//else{ std::cout << &block.baseAddress + i << " :" << (unsigned int)&block.buffer[i] << "\n"; }
			}


			//std::cout << &block.baseAddress << " :" << (int)&block.buffer << "\n";

			fullSize += block.size;
		}

	}
	std::cout << fullSize / 1000000 << "MB occupied\n";
}