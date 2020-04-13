#include "MainWindow.h"

#pragma comment (lib,"ArchieverL.lib")

using namespace std;

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	UI::MainWindow& ref = UI::MainWindow::get();

	MSG msg = {};

	while (GetMessageW(&msg, nullptr, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return 0;
}

//_CrtDumpMemoryLeaks();