#include "MemoryScanner.h"

HANDLE MemoryScanner::getProcessHandle(DWORD pid)
{
	HANDLE result = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (result == NULL)
	{
		return NULL;
	}
	else { return result; }
}

std::vector<MemoryScanner::MemoryBlock> MemoryScanner::getMemoryInformation(unsigned char* baseAddress)
{
	MEMORY_BASIC_INFORMATION memoryInfo;
	std::vector<MemoryScanner::MemoryBlock> result;
	while (VirtualQueryEx(process, baseAddress, &memoryInfo, sizeof(memoryInfo)) != 0)
	{
		if ((memoryInfo.Protect & PAGE_READWRITE) && memoryInfo.State == MEM_COMMIT) { result.push_back({&memoryInfo }); }
		baseAddress = (unsigned char*)memoryInfo.BaseAddress + memoryInfo.RegionSize;
	}
	return result;
}

boolean MemoryScanner::updateMemoryBlock(MemoryBlock &block, SIZE_T &bytesRead)
{
	if(ReadProcessMemory(process, block.baseAddress, block.buffer, block.size, &bytesRead) != 0)
	{
		return true;
	}
	return false;
}

void MemoryScanner::initMemorySearch(void* valueToSearch, short sizeOfValue, SIZE_T &bytesRead)
{
	short shortVal = 0;
	int intVal = 0;
	double doubleVal = 0;
	MemoryScanner::Match temp;

	std::vector<MemoryScanner::Match> result;
	for (auto& block : blocks)
	{
		if (MemoryScanner::updateMemoryBlock(block, bytesRead))
		{
			for (int i = 0; i < bytesRead; i+=sizeOfValue)
			{
				temp = searchInBuffer(valueToSearch, sizeOfValue, bytesRead, block, i);
				if (temp.valueSize != 0) result.push_back(temp);
				/*
				default:
				{
					// TODO: Fix string search
					bool stringMatched = false;
					char* temp = (char*)valueToSearch;
					for (int j = 0; j < block.size; j++) {
						for(int k = 0; k < sizeOfValue; k++)
						{
							if(temp[k] == (char)block.buffer[j + k])
							{
								stringMatched = true;
							}
							else { stringMatched = false; break; }
						}
						if (stringMatched == true) result.push_back(MemoryScanner::Match(i, sizeOfValue, block));
					}
					break;
				}
				}*/
			}
		}
	}
	matches = result;
}

void MemoryScanner::MemorySearch(void* valueToSearch, short sizeOfValue, SIZE_T& bytesRead)
{
	std::vector<MemoryScanner::Match> result;
	MemoryScanner::Match temp;
	short shortVal = 0;
	int intVal = 0;
	double doubleVal = 0;
	for(auto b : blocks)
	{
		MemoryScanner::updateMemoryBlock(b, bytesRead);
	}
	for (auto& match : matches)
	{
		if (match.block.buffer != NULL)
		{
			temp = searchInBuffer(valueToSearch, sizeOfValue, bytesRead, match.block, match.offset);
			if(temp.valueSize != 0) result.push_back(temp);
			/*
			default:
			{
				// TODO: Fix string search
				bool stringMatched = false;
				const char* temp = (const char*)valueToSearch;
				for (int j = 0; j < match.block.size; j++) {
					for (int k = 0; k < sizeOfValue; k++)
					{
						if (temp[k] == (char)match.block.buffer[j + k])
						{
							stringMatched = true;
						}
						else { stringMatched = false; break; }
					}
					if (stringMatched == true) result.push_back(MemoryScanner::Match(match.offset, sizeOfValue, match.block));
				}
				break;
			}
			}*/
		}
	}
	matches = result;
}

//Cleaning up the 2 search functions, by handling the search in one function => removing duplicate code. Has to be fixed
MemoryScanner::Match MemoryScanner::searchInBuffer(void* valueToSearch, short sizeOfValue, SIZE_T bytesRead, MemoryScanner::MemoryBlock block, int offset)
{
	short shortVal = 0;
	int intVal = 0;
	double doubleVal = 0;
	unsigned char shortTempBuffer[2] = { block.buffer[offset], block.buffer[offset + 1] };
	unsigned char intTempBuffer[4] = { block.buffer[offset], block.buffer[offset + 1], block.buffer[offset + 2], block.buffer[offset + 3] };
	unsigned char doubleTempBuffer[8] = { block.buffer[offset], block.buffer[offset + 1], block.buffer[offset + 2], block.buffer[offset + 3], block.buffer[offset + 4],
	block.buffer[offset + 5], block.buffer[offset + 6], block.buffer[offset + 7] };
	MemoryScanner::Match result;

	switch (sizeOfValue)
	{
	case sizeof(char) :
		if (*(char*)valueToSearch == (char)block.buffer[offset]) { result = (MemoryScanner::Match(offset, sizeOfValue, block)); }
		break;
	case sizeof(short) :
		std::memcpy(&shortVal, shortTempBuffer, sizeof(short));
		if (*(short*)valueToSearch == shortVal) { result = (MemoryScanner::Match(offset, sizeOfValue, block)); }
		break;
	case sizeof(int) :
		std::memcpy(&intVal, intTempBuffer, 4);
		if (*(int*)valueToSearch == intVal) 
		{ result = (MemoryScanner::Match(offset, sizeOfValue, block)); }
		break;
	case sizeof(double) :
		std::memcpy(&doubleVal, doubleTempBuffer, sizeof(double));
		if (*(double*)valueToSearch == doubleVal) { result = (MemoryScanner::Match(offset, sizeOfValue, block)); }
		break;
	}

	return result;
}

int MemoryScanner::search(void* valueToSearch, short sizeOfValue, SIZE_T& bytesRead)
{
	if(matches.empty())
	{
		initMemorySearch(valueToSearch, sizeOfValue, bytesRead);
	}
	else
	{
		MemorySearch(valueToSearch, sizeOfValue, bytesRead);
	}
	return matches.size();
}

int MemoryScanner::writeToAllMatches(void* valueToWrite, short sizeOfValue, SIZE_T& bytesRead) 
{
	for (auto& match : matches) 
	{
		switch (sizeOfValue) 
		{
		case sizeof(char) :
			WriteProcessMemory(process, match.block.baseAddress + match.offset, (char*)valueToWrite, sizeof(char), &bytesRead);
			break;
		case sizeof(short) :
			WriteProcessMemory(process, match.block.baseAddress + match.offset, (short*)valueToWrite, sizeof(short), &bytesRead);
			break;
		case sizeof(int) :
			WriteProcessMemory(process, match.block.baseAddress + match.offset, (int*)valueToWrite, sizeof(int), &bytesRead);
			break;
		case sizeof(double) :
			WriteProcessMemory(process, match.block.baseAddress + match.offset, (double*)valueToWrite, sizeof(double), &bytesRead);
			break;
		default:
			//WriteProcessMemory(process, match.block.baseAddress + match.offset, (const char*)valueToWrite, sizeOfValue, &bytesRead);
			break;
		}
	}
	return 0;
}

void MemoryScanner::memDump(MemoryScanner::Match m)
{
	unsigned char* targetAddress = m.block.baseAddress + m.offset;
	//std::vector<unsigned char> buffer(m.valueSize);
	SIZE_T bytesRead;

	std::cout << "Debug: Base Address: " << std::hex << reinterpret_cast<uintptr_t>(m.block.baseAddress)
		<< ", Offset: " << m.offset << ", Target Address: " << reinterpret_cast<uintptr_t>(targetAddress) << std::endl;
}