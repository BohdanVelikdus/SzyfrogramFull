#include "MemoryUtilities.h"

#include <iostream>
#include <string>
#include <exception>
#include <cctype>
#include <cstring>
#include <memory>
#include <sstream>
#include <chrono>
#include <string_view>
#include <algorithm>
#include <vector>
#include <map>
#include <memory>

#include <charconv>
#include <cstdint>


HeapChecker::~HeapChecker()
{
	if (_CrtCheckMemory() == 0) {
		std::ostringstream output;
		output << "### HEAP CORRUPTION DETECTED ###" << "\n";
		output << "\tIn function -> " << m_pFunction << "\n";
		output << "\tIn file-> " << m_pFile << "\n";
		OutputDebugStringA(output.str().c_str());
	}
}

MemoryCheckpoint::MemoryCheckpoint(const char* pFile, const char* pFunction)
{
	m_pFile = pFile;
	m_pFunction = pFunction;
	_CrtMemCheckpoint(&m_Begin);
}


MemoryCheckpoint::~MemoryCheckpoint()
{
	_CrtMemState end, diff;
	_CrtMemCheckpoint(&end);
	if (_CrtMemDifference(&diff, &m_Begin, &end) == 1) {
		OutputDebugStringA(std::format("Leaks detected\n\tIn file ->{} \n\tIn Func ->{}", m_pFile, m_pFunction).c_str());  
		_CrtMemDumpAllObjectsSince(&m_Begin);
		_CrtMemDumpStatistics(&diff);
	}
}

void printMemoryUsage() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        std::cout << "Working set size: " << pmc.WorkingSetSize / 1024 << " KB\n";
        std::cout << "Private usage: " << pmc.PrivateUsage / 1024 << " KB\n";
    }
}
