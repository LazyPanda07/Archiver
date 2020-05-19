#pragma once

#include <Windows.h>
#include <string>
#include <condition_variable>

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

		std::wstring& archiveNameOut;
		std::condition_variable& synchronization;

		bool isSet;

	public:
		//default width and height is half of main window width and height
		ArchiveSettingsWindow(HWND mainWindow, std::wstring& archiveNameOut,std::condition_variable& synchronization, __int32 width = 0, __int32 height = 0);

		HWND getWrapperHWND();

		HWND getArchiveNameHWND();

		void ready();

		bool isReady();

		void synchronize();

		std::wstring& getArchiveNameOut();

		~ArchiveSettingsWindow();
	};
}