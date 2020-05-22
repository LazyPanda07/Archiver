#include "ArchiveSettingsWindow.h"

#include <commctrl.h>

#include "UtilityFunctions.h"
#include "Constants.h"

using namespace std;

#pragma comment (lib,"Comctl32.lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

LRESULT __stdcall SettingsProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

HWND globalMainWindow;
UI::ArchiveSettingsWindow* globalPtr;
mutex archiveMutex;

void createProgressBar(utility::SynchronizationHelper* synchronization);

namespace UI
{
	ArchiveSettingsWindow::ArchiveSettingsWindow(HWND mainWindow, wstring& archiveNameOut, utility::SynchronizationHelper* synchronization, __int32 width, __int32 height) :
		archiveNameOut(archiveNameOut), synchronization(synchronization)
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
			WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME | WS_VISIBLE,
			center.x, center.y,
			width, height,
			nullptr,
			HMENU(),
			nullptr,
			nullptr
		);

		RECT wrapperSizes;
		GetClientRect(wrapper, &wrapperSizes);

		archiveNameHelper = CreateWindowExW
		(
			NULL,
			L"STATIC",
			L"Имя архива:",
			WS_CHILDWINDOW | WS_VISIBLE | SS_SIMPLE,
			wrapperSizes.left + sideOffset, wrapperSizes.top + topOffset,
			settingsStaticWidth, settingsStaticHeight,
			wrapper,
			HMENU(),
			nullptr,
			nullptr
		);

		archiveName = CreateWindowExW
		(
			NULL,
			L"EDIT",
			nullptr,
			WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
			wrapperSizes.left + sideOffset, wrapperSizes.top + topOffset + settingsStaticHeight,
			settingsEditWidth, settingsEditHeight,
			wrapper,
			HMENU(),
			nullptr,
			nullptr
		);

		okButton = CreateWindowExW
		(
			NULL,
			L"BUTTON",
			L"ОК",
			WS_CHILDWINDOW | WS_VISIBLE,
			wrapperSizes.right - 2 * settingsButtonsWidth - sideOffset, wrapperSizes.bottom - settingsButtonsHeight - bottomOffset,
			settingsButtonsWidth, settingsButtonsHeight,
			wrapper,
			HMENU(okSettingsE),
			nullptr,
			nullptr
		);

		cancelButton = CreateWindowExW
		(
			NULL,
			L"BUTTON",
			L"Отмена",
			WS_CHILDWINDOW | WS_VISIBLE,
			wrapperSizes.right - settingsButtonsWidth - sideOffset, wrapperSizes.bottom - settingsButtonsHeight - bottomOffset,
			settingsButtonsWidth, settingsButtonsHeight,
			wrapper,
			HMENU(cancelSettingsE),
			nullptr,
			nullptr
		);

		globalMainWindow = mainWindow;

		SendMessageW(wrapper, WM_COMMAND, lockMainWindowE, NULL);

		globalPtr = this;
	}

	HWND ArchiveSettingsWindow::getWrapperHWND()
	{
		return wrapper;
	}

	HWND ArchiveSettingsWindow::getArchiveNameHWND()
	{
		return archiveName;
	}

	void ArchiveSettingsWindow::ready()
	{
		synchronization->sState = true;
	}

	bool ArchiveSettingsWindow::isReady()
	{
		return synchronization->sState;
	}

	void ArchiveSettingsWindow::synchronize()
	{
		synchronization->sVar.notify_one();
	}

	std::wstring& ArchiveSettingsWindow::getArchiveNameOut()
	{
		return archiveNameOut;
	}

	utility::SynchronizationHelper* ArchiveSettingsWindow::getSynchronizationHelper()
	{
		return synchronization;
	}

	ArchiveSettingsWindow::~ArchiveSettingsWindow()
	{
		UnregisterClassW(L"SettingsDialog", nullptr);
		DestroyWindow(wrapper);
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
			SetWindowPos(globalMainWindow, HWND_TOP, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);

			break;

		case okSettingsE:
			globalPtr->getArchiveNameOut().resize(GetWindowTextLengthW(globalPtr->getArchiveNameHWND()) + 1);
			GetWindowTextW(globalPtr->getArchiveNameHWND(), globalPtr->getArchiveNameOut().data(), globalPtr->getArchiveNameOut().size());
			globalPtr->getArchiveNameOut().pop_back();
			globalPtr->getArchiveNameOut().append(L".mfa");

			globalPtr->ready();
			globalPtr->synchronize();

			thread
			([]
				{
					createProgressBar(globalPtr->getSynchronizationHelper());

					MSG msg = {};

					while (GetMessageW(&msg, nullptr, NULL, NULL))
					{
						TranslateMessage(&msg);
						DispatchMessageW(&msg);
					}
				}
			).detach();

			delete globalPtr;

			break;

		case cancelSettingsE:
			SendMessageW(hwnd, WM_DESTROY, NULL, NULL);

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

void createProgressBar(utility::SynchronizationHelper* synchronization)
{
	RECT rcClient;
	int cyVScroll;
	HWND progressBar;

	InitCommonControls();

	GetClientRect(globalMainWindow, &rcClient);

	cyVScroll = GetSystemMetrics(SM_CYVSCROLL);

	progressBar = CreateWindowExW
	(
		NULL,
		PROGRESS_CLASS,
		nullptr,
		WS_CHILDWINDOW | WS_VISIBLE,
		rcClient.left, rcClient.bottom - cyVScroll,
		rcClient.right, cyVScroll,
		globalMainWindow,
		HMENU(),
		nullptr,
		nullptr
	);

	unique_lock<mutex> dataLock(archiveMutex);

	synchronization->sVar.wait(dataLock, [&synchronization] {return synchronization->sState; });

	size_t filesCount = *reinterpret_cast<size_t*>(synchronization->sData);

	dataLock.unlock();

	SendMessageW(progressBar, PBM_SETRANGE, NULL, MAKELPARAM(0, filesCount));

	SendMessageW(progressBar, PBM_SETSTEP, 1, NULL);

	for (size_t i = 0; i < filesCount; i++)
	{
		unique_lock<mutex> lock(archiveMutex);

		synchronization->sVar.wait(lock, [&synchronization] {return synchronization->sState; });

		SendMessageW(progressBar, PBM_STEPIT, NULL, NULL);

		synchronization->sState = false;
		synchronization->sVar.notify_one();
	}

	SendMessageW(globalMainWindow, endOfArchivingE, reinterpret_cast<WPARAM>(progressBar), NULL);
}