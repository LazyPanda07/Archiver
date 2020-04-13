#include "MainWindow.h"

#include <array>
#include <string>
#include <unordered_map>

#include "Constants.h"
#include "BinaryFile.h"

using namespace std;

static array<wchar_t, 256> fileNameBuffer{};
static unordered_map<wstring, wstring> variants;	//file name - absolute path

LRESULT __stdcall MainWindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

#pragma region Positioning
POINT centerCoordinates(HWND window = GetDesktopWindow());

POINT centerCoordinates(LONG width, LONG height, HWND window = GetDesktopWindow());

#pragma endregion

#pragma region ButtonsEvents
void chooseFileEvent(HWND availableListBox, HWND addedListBox);

void deleteFileEvent(HWND availableListBox, HWND addedListBox);

void addFileEvent(HWND availableListBox, HWND addedListBox);

void encryptFilesEvent(HWND addedListBox);

void decryptFilesEvent();

#pragma endregion

namespace UI
{
	MainWindow::MainWindow() : currentDirectory(filesystem::current_path())
	{
		WNDCLASSEXW wnd = {};
		POINT windowPos = centerCoordinates(mainWindowWidth, mainWindowHeight);
		RECT sizes;
		LONG width;
		LONG height;

		wnd.cbSize = sizeof(WNDCLASSEXW);
		wnd.lpfnWndProc = MainWindowProcedure;
		wnd.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
		wnd.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wnd.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
		wnd.lpszClassName = L"MainWindow";

		RegisterClassExW(&wnd);

		window = CreateWindowExW
		(
			NULL,
			wnd.lpszClassName,
			L"Archiever",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			windowPos.x, windowPos.y,
			mainWindowWidth, mainWindowHeight,
			nullptr,
			HMENU(),
			nullptr,
			nullptr
		);

		GetClientRect(window, &sizes);
		width = sizes.right - sizes.left;
		height = sizes.bottom - sizes.top;

		addFileButton = CreateWindowExW
		(
			NULL,
			L"BUTTON",
			L"�������� ����",
			WS_CHILDWINDOW | WS_VISIBLE,
			0, 0,
			controlButtonsWidth, controlButtonsHeight,
			window,
			HMENU(addFileE),
			nullptr,
			nullptr
		);

		chooseFileButton = CreateWindowExW
		(
			NULL,
			L"BUTTON",
			L"������� ����",
			WS_CHILDWINDOW | WS_VISIBLE,
			controlButtonsWidth, 0,
			controlButtonsWidth, controlButtonsHeight,
			window,
			HMENU(chooseFileE),
			nullptr,
			nullptr
		);

		deleteFileButton = CreateWindowExW
		(
			NULL,
			L"BUTTON",
			L"������� ����",
			WS_CHILDWINDOW | WS_VISIBLE,
			controlButtonsWidth * 2, 0,
			controlButtonsWidth, controlButtonsHeight,
			window,
			HMENU(deleteFileE),
			nullptr,
			nullptr
		);

		encryptFilesButton = CreateWindowExW
		(
			NULL,
			L"BUTTON",
			L"������������ �����",
			WS_CHILDWINDOW | WS_VISIBLE,
			controlButtonsWidth * 3, 0,
			controlButtonsWidth, controlButtonsHeight,
			window,
			HMENU(encryptFilesE),
			nullptr,
			nullptr
		);

		decryptFilesButton = CreateWindowExW
		(
			NULL,
			L"BUTTON",
			L"������� �����",
			WS_CHILDWINDOW | WS_VISIBLE,
			controlButtonsWidth * 4, 0,
			controlButtonsWidth + 50, controlButtonsHeight,
			window,
			HMENU(decryptFilesE),
			nullptr,
			nullptr
		);

		availableFiles = CreateWindowExW
		(
			WS_EX_CLIENTEDGE,
			L"LISTBOX",
			nullptr,
			WS_CHILDWINDOW | WS_VISIBLE | LBS_SORT | LBS_EXTENDEDSEL | LBS_MULTIPLESEL,
			0, topOffset + informationMessagesHeight,
			width / 2, height,
			window,
			HMENU(),
			nullptr,
			nullptr
		);

		addedFiles = CreateWindowExW
		(
			WS_EX_CLIENTEDGE,
			L"LISTBOX",
			nullptr,
			WS_CHILDWINDOW | WS_VISIBLE | LBS_SORT | LBS_EXTENDEDSEL | LBS_MULTIPLESEL,
			width / 2, topOffset + informationMessagesHeight,
			width / 2, height,
			window,
			HMENU(),
			nullptr,
			nullptr
		);

		availableArea = CreateWindowExW
		(
			NULL,
			L"STATIC",
			L"������� ��� ������ ������",
			WS_CHILDWINDOW | WS_VISIBLE | SS_CENTER,
			width / 4 - informationMessagesWidth / 2, topOffset,
			informationMessagesWidth, informationMessagesHeight,
			window,
			HMENU(),
			nullptr,
			nullptr
		);

		addedArea = CreateWindowExW
		(
			NULL,
			L"STATIC",
			L"������� � ��� ���������� �������",
			WS_CHILDWINDOW | WS_VISIBLE | SS_CENTER,
			width / 2 + width / 4 - informationMessagesWidth / 2, topOffset,
			informationMessagesWidth, informationMessagesHeight,
			window,
			HMENU(),
			nullptr,
			nullptr
		);

		filesystem::directory_iterator it(currentDirectory);

		for (auto&& i : it)
		{
			variants[i.path().filename().generic_wstring()] = i.path().generic_wstring();

			SendMessageW(availableFiles, LB_ADDSTRING, NULL, reinterpret_cast<LPARAM>(i.path().filename().generic_wstring().data()));
		}

		SendMessageW(window, initListBoxes, reinterpret_cast<WPARAM>(availableFiles), reinterpret_cast<LPARAM>(addedFiles));
	}

	MainWindow::~MainWindow()
	{
		DestroyWindow(chooseFileButton);
		DestroyWindow(deleteFileButton);
		DestroyWindow(window);
	}

	MainWindow& MainWindow::get()
	{
		static MainWindow instance;

		return instance;
	}

	HWND MainWindow::getHWND() const
	{
		return window;
	}

	void MainWindow::resize()
	{

	}
}

LRESULT __stdcall MainWindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HWND lastButtonClicked;
	static HWND availableListBox;
	static HWND addedListBox;

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);

		return 0;

	case WM_COMMAND:
		if (HIWORD(wparam) == BN_CLICKED)
		{
			lastButtonClicked = reinterpret_cast<HWND>(lparam);
		}

		switch (wparam)
		{
		case chooseFileE:
			chooseFileEvent(availableListBox, addedListBox);

			break;

		case addFileE:
			addFileEvent(availableListBox, addedListBox);;

			break;

		case deleteFileE:
			deleteFileEvent(availableListBox, addedListBox);

			break;

		case encryptFilesE:
			encryptFilesEvent(addedListBox);

			break;

		case decryptFilesE:
			decryptFilesEvent();

			break;
		}

		return 0;

	case initListBoxes:
		availableListBox = reinterpret_cast<HWND>(wparam);
		addedListBox = reinterpret_cast<HWND>(lparam);

	default:
		return DefWindowProcW(hwnd, msg, wparam, lparam);
	}
}

POINT centerCoordinates(HWND window)
{
	RECT sizes;

	GetClientRect(window, &sizes);

	return { static_cast<LONG>((sizes.right - sizes.left) * 0.5),static_cast<LONG>((sizes.bottom - sizes.top) * 0.5) };
}

POINT centerCoordinates(LONG width, LONG height, HWND window)
{
	RECT sizes;

	GetClientRect(window, &sizes);

	LONG x = (sizes.right - sizes.left) * 0.5;
	LONG y = (sizes.bottom - sizes.top) * 0.5;

	return { static_cast<LONG>(x - width * 0.5),static_cast<LONG>(y - height * 0.5) };
}

void chooseFileEvent(HWND availableListBox, HWND addedListBox)
{
	array<__int32, 1024> indices;

	LRESULT elements = SendMessageW(availableListBox, LB_GETSELITEMS, 1024, reinterpret_cast<LPARAM>(indices.data()));

	if (elements != LB_ERR)
	{
		array<wchar_t, 128> fileName;
		for (size_t i = 0; i < elements; i++)
		{
			if (SendMessageW(availableListBox, LB_GETTEXT, indices[i], reinterpret_cast<LPARAM>(fileName.data())) != LB_ERR)
			{
				SendMessageW(addedListBox, LB_ADDSTRING, NULL, reinterpret_cast<LPARAM>(fileName.data()));
			}
		}

		for (int i = elements - 1; i >= 0; i--)
		{
			SendMessageW(availableListBox, LB_DELETESTRING, indices[i], NULL);
		}
	}
}

void deleteFileEvent(HWND availableListBox, HWND addedListBox)
{
	array<__int32, 1024> indices;

	LRESULT elements = SendMessageW(addedListBox, LB_GETSELITEMS, 1024, reinterpret_cast<LPARAM>(indices.data()));

	if (elements != LB_ERR)
	{
		array<wchar_t, 128> fileName;
		for (size_t i = 0; i < elements; i++)
		{
			if (SendMessageW(addedListBox, LB_GETTEXT, indices[i], reinterpret_cast<LPARAM>(fileName.data())) != LB_ERR)
			{
				SendMessageW(availableListBox, LB_ADDSTRING, NULL, reinterpret_cast<LPARAM>(fileName.data()));
			}
		}

		for (int i = elements - 1; i >= 0; i--)
		{
			SendMessageW(addedListBox, LB_DELETESTRING, indices[i], NULL);
		}
	}
}

void addFileEvent(HWND availableListBox, HWND addedListBox)
{
	OPENFILENAMEW file = {};

	std::wstring temp = fileNameBuffer.data();

	file.lStructSize = sizeof(OPENFILENAMEW);
	file.hwndOwner = GetParent(availableListBox);
	file.lpstrFile = fileNameBuffer.data();
	file.lpstrFile[0] = L'\0';
	file.nMaxFile = sizeof(fileNameBuffer);
	file.lpstrFilter = L"��� �����\0*.*\0\0";
	file.nFilterIndex = 1;
	file.lpstrFileTitle = nullptr;
	file.nMaxFileTitle = 0;
	file.lpstrInitialDir = nullptr;
	file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	GetOpenFileNameW(&file);

	std::wstring_view absolutePath = file.lpstrFile;
	std::wstring_view fileName = absolutePath.substr(absolutePath.rfind(L'\\') + 1);

	if (fileName == L"")
	{
		return;
	}

	variants[fileName.data()] = absolutePath;

	SendMessageW(addedListBox, LB_ADDSTRING, NULL, reinterpret_cast<LPARAM>(fileName.data()));
}

void encryptFilesEvent(HWND addedListBox)
{
	LRESULT elements = SendMessageW(addedListBox, LB_GETCOUNT, NULL, NULL);

	if (elements != LB_ERR)
	{
		vector<wstring> files;
		files.reserve(elements);

		for (size_t i = 0; i < elements; i++)
		{
			files.emplace_back(wstring());
			LRESULT temSize = SendMessageW(addedListBox, LB_GETTEXTLEN, i, NULL);

			files.back().resize(temSize);

			SendMessageW(addedListBox, LB_GETTEXT, i, reinterpret_cast<LPARAM>(files[i].data()));
		}

		vector<wstring> fullPathFiles(elements);

		for (size_t i = 0; i < elements; i++)
		{
			fullPathFiles[i] = variants.find(files[i])->second;
		}

		BinaryFile::encodeBinaryFile(fullPathFiles, L"test.bin");

		MessageBoxW(GetParent(addedListBox), L"����� ������� ������", L"����������", MB_OK);
	}
}

void decryptFilesEvent()
{
	OPENFILENAMEW file = {};

	std::wstring temp = fileNameBuffer.data();

	file.lStructSize = sizeof(OPENFILENAMEW);
	file.hwndOwner = nullptr;
	file.lpstrFile = fileNameBuffer.data();
	file.lpstrFile[0] = L'\0';
	file.nMaxFile = sizeof(fileNameBuffer);
	file.lpstrFilter = L"�������� �����\0*.bin\0\0";
	file.nFilterIndex = 1;
	file.lpstrFileTitle = nullptr;
	file.nMaxFileTitle = 0;
	file.lpstrInitialDir = nullptr;
	file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	GetOpenFileNameW(&file);

	std::wstring fileName = file.lpstrFile;

	BinaryFile::decodeBinaryFile(fileName);

	MessageBoxW(nullptr, L"���������� ������ �������", L"����������", MB_OK);
}