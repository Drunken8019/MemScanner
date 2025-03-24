// MemScann.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "MemoryScanner.h"
#include <iostream>
#include <map>
#include <string>

typedef void (*functionReference) (void);

static void printAllProcesses();
static void printHeader();
static void printHelp();
static void runSearch();
static void saveMatch(MemoryScanner::Match m);
static void write();

std::map<std::string, functionReference> commands = { {"search", (functionReference) runSearch}, {"tasklist", (functionReference) printAllProcesses},
	{"help", (functionReference)printHelp}, {"write", (functionReference)write} };
std::vector<MemoryScanner::Match> savedMatches;
MemoryScanner m1;

int main(int argc, char* argv)
{
	printHeader();
	std::string in = "";
	//---------------CLI----------------
	do
	{
		std::cout << "memscan>";
		std::cin >> in;
		if(commands.find(in) != commands.end())
		{
			commands[in]();
		}
		else if (in.compare("exit") != 0) std::cout << "\"" + in + "\" " + "is not recognized as a command. Use \"help\" to view all available commands." << std::endl;
	} while (in.compare("exit") != 0);
}

static void printAllProcesses()
{
	if (system("tasklist") != 0)
	{
		std::cout << "Task list could not be printed" << std::endl;
	}
	std::cout << std::endl;
}

static void printHeader()
{
	std::cout << "##     ##         ##     ## ######## ##     ##" << std::endl;
	std::cout << " ##   ##          ###   ### ##       ###   ###" << std::endl;
	std::cout << "  ## ##           #### #### ##       #### ####" << std::endl;
	std::cout << "   ###    ####### ## ### ## ######   ## ### ##" << std::endl;
	std::cout << "  ## ##           ##     ## ##       ##     ##" << std::endl;
	std::cout << " ##   ##          ##     ## ##       ##     ##" << std::endl;
	std::cout << "##     ##         ##     ## ######## ##     ##" << std::endl;
	std::cout << "A memory scanner." << std::endl;
	std::cout << std::endl;
}

static void printHelp()
{
	printf("* %-15s -> Prints info on all commands.", "help"); std::cout << std::endl;
	printf("* %-15s -> Prints info on all commands.", "tasklist"); std::cout << std::endl;
	printf("* %-15s -> Close memory scanner.", "exit"); std::cout << std::endl;
	printf("* %-15s -> Starts a new search.", "search"); std::cout << std::endl;
	printf("**  %-15s -> Value to search.", "[value]"); std::cout << std::endl;
	printf("**  %-15s -> End search.", "exit"); std::cout << std::endl;
	printf("**  %-15s -> Print all adresses of current matches", "printAdr"); std::cout << std::endl;
	printf("**  %-15s -> Starts a write operation.", "write"); std::cout << std::endl;
	printf("***   %-15s -> Value to write.", "[value]"); std::cout << std::endl;
	printf("**  %-15s -> Saves all current matches. Can be retrieved with \"saved\"", "save"); std::cout << std::endl;
	printf("* %-15s -> Starts a new writing operation.", "write"); std::cout << std::endl;
	printf("**  %-15s -> Target adress(es) to be written to.", "[target]"); std::cout << std::endl;
	printf("***   %-15s -> Set all saved adresses as target.", "saved"); std::cout << std::endl;
	printf("**  %-15s -> Value to write.", "[value]"); std::cout << std::endl;
}

static void runSearch()
{
	DWORD pid;
	SIZE_T bytesRead;
	std::map<std::string, int> searchCommands{ {"printAdr", 0}, {"write", 1}, {"help", 2}, { "save", 3 }, {"exit", 4}};
	//int d = 0;
	int valSize = 0;
	std::string in = "";
	std::string toWrite = "";
	std::cout << "memscan search> Enter PID: ";
	std::cin >> pid;
	m1 = { pid };

	std::cout << "memscan search> Enter value size: ";
	std::cin >> valSize;
	void* d = 0;
	void* writeContent = 0;

	int matchCount = 0;

	do		
	{
		std::cout << "memscan search>";
		std::cin >> in;

		if (searchCommands.find(in) != searchCommands.end())
		{
			switch (searchCommands[in])
			{
			case 0:
				if (matchCount == 0) std::cout << "No adresses to print." << std::endl;
				else 
				{
					for(auto& match : m1.matches)
					{
						m1.memDump(match);
					}
				}
				break;

			case 1:
				std::cout << "memscan write> Value: ";
				std::cin >> toWrite;
				switch (valSize) 
				{
				case sizeof(char) :
					writeContent = new char(toWrite.at(0));
					break;
				case sizeof(short) :
					writeContent = new short(std::stoi(toWrite));
					break;
				case sizeof(int) :
					writeContent = new int(std::stoi(toWrite));
					break;
				case sizeof(double) :
					writeContent = new double(std::stod(toWrite));
					break;
				}
				m1.writeToAllMatches(writeContent, valSize, bytesRead);
				break;

			case 2:
				printHelp();
				break;

			case 3:
				for (auto& a : m1.matches)
				{
					saveMatch(a);
				}
				std::cout << m1.matches.size() << " match(es) saved." << std::endl;
				break;

			case 4:
				break;
			}
		}
		else 
		{	
			switch (valSize) 
			{
			case sizeof(char) :
				d = new char(in.at(0));
				break;
			case sizeof(short) :
				d = new short(std::stoi(in));
				break;
			case sizeof(int) :
				d = new int(std::stoi(in));
				break;
			case sizeof(double) :
				d = new double(std::stod(in));
				break;
			default:
				// TODO: Fix string search
				//d = (void*)in.c_str();
				break;
			}
			matchCount = m1.search(d, valSize, bytesRead);
			std::cout << "Match count: " << matchCount << "\n";
		}
	} while (in.compare("exit") != 0);

	delete(d);
	delete(writeContent);
}

static void write()
{
	std::string in = "";
	std::cout << "memscan write> Target: ";
	std::cin >> in;
	int writeContent = 0;
	if(in.compare("saved") == 0)
	{
		std::cout << "memscan write> Value: ";
		std::cin >> in;
		writeContent = std::stoi(in);
		for(auto& a : savedMatches)
		{
			WriteProcessMemory(m1.process, a.block.baseAddress + a.offset, &writeContent, sizeof(writeContent), NULL);
		}
	}
	else
	{
		std::cout << "Only writing to saved targets is supported for now." << std::endl;
	}
}

static void saveMatch(MemoryScanner::Match m)
{
	savedMatches.push_back(m);
}