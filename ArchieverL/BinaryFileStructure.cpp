#include "BinaryFileStructure.h"

namespace BinaryFile
{
	BinaryFileStructure::BinaryFileStructure(short importantBitsCount, unsigned __int64 sizeInBytes) : importantBitsCount(importantBitsCount), sizeInBytes(sizeInBytes)
	{
		memset(fileName, '\0', sizeof(fileName));
		memset(filePath, '\0', sizeof(filePath));
	}

	BinaryFileStructure::BinaryFileStructure(short importantBitsCount, const std::wstring& fileName, unsigned __int64 sizeInBytes, const std::wstring* const filePath) : BinaryFileStructure(importantBitsCount, sizeInBytes)
	{
		wmemcpy(this->fileName, fileName.data(), fileName.size());

		if (filePath)
		{
			wmemcpy(this->filePath, filePath->data(), filePath->size());
		}
	}
}