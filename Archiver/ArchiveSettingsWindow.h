#pragma once

#include <Windows.h>
#include <string>

namespace UI
{
	class ArchiveSettingsWindow
	{
	private:
		HWND wrapper;

		HWND okButton;
		HWND cancelButton;

		HWND archiveNameHelper;	//static
		HWND archiveName;

	public:
		//default width and height is half of main window width and height
		ArchiveSettingsWindow(HWND mainWindow, __int32 width = 0, __int32 height = 0);

		HWND getWrapperHWND();

		HWND getArchiveNameHWND();

		~ArchiveSettingsWindow();
	};
}