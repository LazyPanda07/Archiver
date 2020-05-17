#pragma once

#include <Windows.h>

namespace UI
{
	class ArchiveSettingsWindow
	{
	private:
		HWND wrapper;

	public:
		//default width and height is half of main window width and height
		ArchiveSettingsWindow(HWND mainWindow, int width = 0, int height = 0);

		~ArchiveSettingsWindow();
	};
}