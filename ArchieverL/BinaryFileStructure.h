#pragma once

#include <string>

namespace BinaryFile
{
	struct BinaryFileStructure
	{
		short importantBitsCount;		//2
		wchar_t fileName[59];			//118
		unsigned __int64 sizeInBytes;	//8

		BinaryFileStructure(short importantBitsCount = 0, unsigned __int64 sizeInBytes = 0);

		BinaryFileStructure(short importantBitsCount, const std::wstring& fileName, unsigned __int64 sizeInBytes);
	};
}