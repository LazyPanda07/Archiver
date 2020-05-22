#pragma once

#include <condition_variable>

namespace utility
{
	struct SynchronizationHelper
	{
		std::condition_variable sVar;
		bool sState;
		void* sData;

		SynchronizationHelper() : sState(false), sData(nullptr)
		{

		}
	};
}