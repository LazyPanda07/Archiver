#pragma once

#include <string>
#include <vector>

#include "../Utility/SynchronizationHelper.h"

namespace BinaryFile
{
	void encodeBinaryFile(const std::vector<std::pair<std::wstring, std::vector<std::wstring>>>& binaryFileNameIn, const std::wstring& binaryFileNameOut,utility::SynchronizationHelper* synchronization = nullptr);

	void decodeBinaryFile(const std::wstring& binaryFileName);
}