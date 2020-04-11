#include <iostream>
#include <vector>
#include <fstream>

#include "BinaryFile.h"
#include "BinaryFileStructure.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#endif

using namespace std;

int main(int argc, char** argv)
{
	BinaryFile::encodeBinaryFile({ L"test.txt", L"test1.txt", L"test2.txt" }, L"test.bin");

	BinaryFile::decodeBinaryFile(L"test.bin");

	return 0;
}

//_CrtDumpMemoryLeaks();