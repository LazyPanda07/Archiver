#pragma once

#include <Windows.h>
#include <filesystem>

namespace UI
{
	class MainWindow
	{
	private:
		HWND window;

		HWND chooseFileButton;
		HWND addFileButton;
		HWND deleteFileButton;
		HWND encryptFilesButton;
		HWND decryptFilesButton;
		HWND changeDirectoryButton;

		HWND availableFiles;	//listbox
		HWND addedFiles;		//listbox

		HWND availableArea;		//static
		HWND addedArea;			//static

		std::filesystem::path currentDirectory;

	private:
		MainWindow();

		MainWindow(const MainWindow&) = delete;
		MainWindow(MainWindow&&) = delete;
		MainWindow& operator = (const MainWindow&) = delete;
		MainWindow& operator = (MainWindow&&) = delete;

		~MainWindow();

	public:
		static MainWindow& get();

		HWND getHWND() const;

		void resize();

		void setCurrentDirectory(const std::wstring& path);

		const std::filesystem::path& getCurrentDirectory() const;
	};
}