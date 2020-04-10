#pragma once

#include <string>

namespace BinaryFile
{
	void encodeBinaryFile(const std::wstring& binaryFileNameIn, const std::wstring& binaryFileNameOut);

	void decodeBinaryFile(const std::wstring& binaryFileName);
}