#include "ArchiveSettingsWindow.h"

#include "UtilityFunctions.h"
#include "Constants.h"

using namespace std;

LRESULT __stdcall SettingsProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

HWND globalMainWindow;

namespace UI
{
	ArchiveSettingsWindow::ArchiveSettingsWindow(HWND mainWindow, int width, int height)
	{
		WNDCLASSEXW dialog = {};

		dialog.cbSize = sizeof(WNDCLASSEXW);
		dialog.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		dialog.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
		dialog.lpszClassName = L"SettingsDialog";
		dialog.lpfnWndProc = SettingsProcedure;
		dialog.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));

		RegisterClassExW(&dialog);

		RECT sizes;
		RECT mainWindowRect;
		GetWindowRect(mainWindow, &mainWindowRect);

		if (!width || !height)
		{
			GetClientRect(mainWindow, &sizes);
		}

		if (!width)
		{
			width = (sizes.right - sizes.left) * 0.5;
		}

		if (!height)
		{
			height = (sizes.bottom - sizes.top) * 0.5;
		}

		const POINT center = { (mainWindowRect.left + mainWindowRect.right - width) / 2,(mainWindowRect.top + mainWindowRect.bottom - height) / 2 };

		wrapper = CreateWindowExW
		(
			NULL,
			dialog.lpszClassName,
			L"Настройки создания архива",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			center.x, center.y,
			width, height,
			nullptr,
			HMENU(),
			nullptr,
			nullptr
		);

		globalMainWindow = mainWindow;

		SendMessageW(wrapper, WM_COMMAND, lockMainWindowE, NULL);
	}

	ArchiveSettingsWindow::~ArchiveSettingsWindow()
	{
		UnregisterClassW(L"SettingsDialog", nullptr);
	}
}

LRESULT __stdcall SettingsProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (wparam)
		{
		case lockMainWindowE:
			EnableWindow(globalMainWindow, false);

			break;

		case unlockMainWindowE:
			EnableWindow(globalMainWindow, true);

			break;
		}

		return 0;

	case WM_DESTROY:
		SendMessageW(hwnd, WM_COMMAND, unlockMainWindowE, NULL);

		return 0;

	default:
		return DefWindowProcW(hwnd, msg, wparam, lparam);
	}
}