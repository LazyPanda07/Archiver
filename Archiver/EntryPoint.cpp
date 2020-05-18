#include "MainWindow.h"
#include "Constants.h"

#pragma comment (lib,"ArchieverL.lib")

using namespace std;

__int32 __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, __int32)
{
	UI::MainWindow& ref = UI::MainWindow::get();

	SendMessageW(ref.getHWND(), initMainWindowPtr, reinterpret_cast<WPARAM>(&ref), NULL);

	MSG msg = {};

	while (GetMessageW(&msg, nullptr, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return 0;
}

//_CrtDumpMemoryLeaks();