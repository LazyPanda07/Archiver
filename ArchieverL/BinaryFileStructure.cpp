#include "BinaryFileStructure.h"

namespace BinaryFile
{
	BinaryFileStructure::BinaryFileStructure(short importantBitsCount, unsigned __int64 sizeInBytes) : importantBitsCount(importantBitsCount), sizeInBytes(sizeInBytes)
	{
		memset(fileName, '\0', sizeof(fileName));
	}

	BinaryFileStructure::BinaryFileStructure(short importantBitsCount, const std::wstring& fileName, unsigned __int64 sizeInBytes) : BinaryFileStructure(importantBitsCount, sizeInBytes)
	{
		wmemcpy(this->fileName, fileName.data(), fileName.size());

		this->fileName[wcslen(this->fileName)] = L'\0';
	}
}