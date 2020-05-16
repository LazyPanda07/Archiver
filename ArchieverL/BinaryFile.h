#pragma once

#include <string>
#include <vector>

namespace BinaryFile
{
	void encodeBinaryFile(const std::vector<std::pair<std::wstring, std::vector<std::wstring>>>& binaryFileNameIn, const std::wstring& binaryFileNameOut);

	void decodeBinaryFile(const std::wstring& binaryFileName);
}