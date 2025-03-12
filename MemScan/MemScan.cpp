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

int main()
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
	int writeContent = 0;
	std::map<std::string, int> searchCommands{ {"printAdr", 0}, {"write", 1}, {"help", 2}, { "save", 3 }, {"exit", 4}};
	int d = 0;
	std::string in = "";
	std::cout << "memscan search> Enter PID: ";
	std::cin >> pid;
	m1 = { pid };

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
				for (auto& a : m1.matches)
				{
					std::cout << (unsigned char)a.block.baseAddress + a.offset << "\n";
				}
				break;

			case 1:
				std::cout << "memscan write> Value: ";
				std::cin >> writeContent;
				for (auto& a : m1.matches)
				{
					WriteProcessMemory(m1.process, a.block.baseAddress + a.offset, &writeContent, sizeof(writeContent), NULL);
				}
				break;

			case 2:
				printHelp();
				break;

			case 3:
				for (auto& a : m1.matches)
				{
					saveMatch(a);
				}
				std::cout << m1.matches.size() << " matche(s) saved." << std::endl;
				break;

			case 4:
				break;
			}
		}
		else 
		{
			d = std::stoi(in);
			matchCount = m1.search(&d, 4, bytesRead);
			std::cout << "Match count: " << matchCount << "\n";
			matchCount = 0;
		}
	} while (in.compare("exit") != 0);
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