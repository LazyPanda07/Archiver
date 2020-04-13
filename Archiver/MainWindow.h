#pragma once

#include <Windows.h>
#include <filesystem>

namespace UI
{
	class MainWindow
	{
	private:
		HWND window;

		HWND addFileButton;
		HWND deleteFileButton;
		HWND chooseFileButton;

		HWND availableFiles;
		HWND addedFiles;

		HWND availableArea;
		HWND addedArea;

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
	};
}