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
	BinaryFile::encodeBinaryFile({ L"advanced.dll", L"app.dll", L"base.dll" }, L"test.bin");

	BinaryFile::decodeBinaryFile(L"test.bin");

	return 0;
}

//_CrtDumpMemoryLeaks();