#pragma once

#include <string>

namespace BinaryFile
{
	struct BinaryFileStructure
	{
		short importantBitsCount;		//2
		wchar_t fileName[32];			//64
		wchar_t filePath[91];			//182
		unsigned __int64 sizeInBytes;	//8

		BinaryFileStructure(short importantBitsCount = 0, unsigned __int64 sizeInBytes = 0);

		BinaryFileStructure(short importantBitsCount, const std::wstring& fileName, unsigned __int64 sizeInBytes, const std::wstring* const filePath = nullptr);
	};
}