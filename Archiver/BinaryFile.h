#pragma once

#include <string>

namespace BinaryFile
{
	struct BinaryFileStructure
	{
		short importantBitsCount;	//2
		wchar_t fileName[16];	//32
		wchar_t filePath[11];	//22
		unsigned __int64 sizeInBytes;	//8

		BinaryFileStructure(short importantBitsCount = 0, unsigned __int64 sizeInBytes = 0);

		BinaryFileStructure(short importantBitsCount, const std::wstring& fileName, unsigned __int64 sizeInBytes, const std::wstring* const filePath = nullptr);
	};

	void encodeBinaryFile(const std::wstring& binaryFileNameIn, const std::wstring& binaryFileNameOut);

	void decodeBinaryFile(const std::wstring& binaryFileName);
}