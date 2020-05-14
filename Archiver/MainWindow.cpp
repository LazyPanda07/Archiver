#include "MainWindow.h"

#include <array>
#include <string>
#include <unordered_map>
#include <shlobj.h>

#include "Constants.h"
#include "BinaryFile.h"

using namespace std;

array<wchar_t, 256> fileNameBuffer{};
unordered_map<wstring, wstring> variants;	//file name - absolute path

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

void changeDirectory(UI::MainWindow& ref);

#pragma endregion

#pragma region Utility
//If last element is folder recursively add all files in this folder and subfolders
void isFolder(vector<wstring>& fullPathFiles);

#pragma endregion


namespace UI
{
	MainWindow::MainWindow()
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
			L"Добавить файл",
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
			L"Выбрать файл",
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
			L"Убрать файл",
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
			L"Архивировать файлы",
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
			L"Извлечь файлы",
			WS_CHILDWINDOW | WS_VISIBLE,
			controlButtonsWidth * 4, 0,
			controlButtonsWidth, controlButtonsHeight,
			window,
			HMENU(decryptFilesE),
			nullptr,
			nullptr
		);

		changeDirectoryButton = CreateWindowExW
		(
			NULL,
			L"BUTTON",
			L"Изменить директорию",
			WS_CHILDWINDOW | WS_VISIBLE,
			controlButtonsWidth * 5, 0,
			controlButtonsWidth, controlButtonsHeight,
			window,
			HMENU(changeDirectoryE),
			nullptr,
			nullptr
		);

		availableFiles = CreateWindowExW
		(
			WS_EX_CLIENTEDGE,
			L"LISTBOX",
			nullptr,
			WS_CHILDWINDOW | WS_VISIBLE | LBS_SORT | LBS_EXTENDEDSEL | LBS_MULTIPLESEL | WS_VSCROLL | LBS_NOINTEGRALHEIGHT,
			0, topOffset + informationMessagesHeight,
			width / 2, height - topOffset - informationMessagesHeight,
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
			WS_CHILDWINDOW | WS_VISIBLE | LBS_SORT | LBS_EXTENDEDSEL | LBS_MULTIPLESEL | WS_VSCROLL | LBS_NOINTEGRALHEIGHT,
			width / 2, topOffset + informationMessagesHeight,
			width / 2, height - topOffset - informationMessagesHeight,
			window,
			HMENU(),
			nullptr,
			nullptr
		);

		availableArea = CreateWindowExW
		(
			NULL,
			L"STATIC",
			L"Выбор файлов для архивирования",
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
			L"Файлы для архивирования",
			WS_CHILDWINDOW | WS_VISIBLE | SS_CENTER,
			width / 2 + width / 4 - informationMessagesWidth / 2, topOffset,
			informationMessagesWidth, informationMessagesHeight,
			window,
			HMENU(),
			nullptr,
			nullptr
		);

		this->setCurrentDirectory(filesystem::current_path());

		SendMessageW(window, initListBoxes, reinterpret_cast<WPARAM>(availableFiles), reinterpret_cast<LPARAM>(addedFiles));
	}

	MainWindow::~MainWindow()
	{
		DestroyWindow(chooseFileButton);
		DestroyWindow(deleteFileButton);
		DestroyWindow(addFileButton);
		DestroyWindow(encryptFilesButton);
		DestroyWindow(decryptFilesButton);
		DestroyWindow(availableFiles);
		DestroyWindow(addedFiles);
		DestroyWindow(availableArea);
		DestroyWindow(addedArea);
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
		RECT sizes;

		GetClientRect(window, &sizes);

		LONG width = sizes.right - sizes.left;
		LONG height = sizes.bottom - sizes.top;

		SetWindowPos(availableArea, HWND_BOTTOM, width / 4 - informationMessagesWidth / 2, topOffset, informationMessagesWidth, informationMessagesHeight, SWP_SHOWWINDOW);
		SetWindowPos(addedArea, HWND_BOTTOM, width / 2 + width / 4 - informationMessagesWidth / 2, topOffset, informationMessagesWidth, informationMessagesHeight, SWP_SHOWWINDOW);
		SetWindowPos(availableFiles, HWND_BOTTOM, 0, topOffset + informationMessagesHeight, width / 2, height - topOffset - informationMessagesHeight, SWP_SHOWWINDOW);
		SetWindowPos(addedFiles, HWND_BOTTOM, width / 2, topOffset + informationMessagesHeight, width / 2, height - topOffset - informationMessagesHeight, SWP_SHOWWINDOW);
	}

	void MainWindow::setCurrentDirectory(const wstring& path)
	{
		variants.clear();

		LRESULT count = SendMessageW(availableFiles, LB_GETCOUNT, NULL, NULL);

		for (size_t i = 0; i < count; i++)
		{
			SendMessageW(availableFiles, LB_DELETESTRING, 0, NULL);
		}

		count = SendMessageW(addedFiles, LB_GETCOUNT, NULL, NULL);

		for (size_t i = 0; i < count; i++)
		{
			SendMessageW(addedFiles, LB_DELETESTRING, 0, NULL);
		}

		currentDirectory = path;

		filesystem::directory_iterator it(currentDirectory);

		for (auto&& i : it)
		{
			filesystem::path path = i.path();

			variants[path.filename().wstring()] = path.wstring();

			SendMessageW(availableFiles, LB_ADDSTRING, NULL, reinterpret_cast<LPARAM>(path.filename().wstring().data()));
		}
	}

	const filesystem::path& MainWindow::getCurrentDirectory() const
	{
		return currentDirectory;
	}
}

LRESULT __stdcall MainWindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HWND lastButtonClicked;
	static HWND availableListBox;
	static HWND addedListBox;
	static UI::MainWindow* ptr = nullptr;

	switch (msg)
	{
	case WM_SIZE:
		if (ptr)
		{
			ptr->resize();
		}

		return 0;

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

		case changeDirectoryE:
			changeDirectory(*ptr);

			break;
		}

		return 0;

	case initListBoxes:
		availableListBox = reinterpret_cast<HWND>(wparam);
		addedListBox = reinterpret_cast<HWND>(lparam);

		return 0;

	case initMainWindowPtr:
		ptr = reinterpret_cast<UI::MainWindow*>(wparam);

		return 0;

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
	file.lpstrFilter = L"Все файлы\0*.*\0\0";
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

	variants.insert(make_pair(fileName, absolutePath));

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
			fullPathFiles[i] = variants[files[i]];
			isFolder(fullPathFiles);
		}

		if (fullPathFiles.size())
		{
			BinaryFile::encodeBinaryFile(fullPathFiles, L"test.mfa");

			MessageBoxW(GetParent(addedListBox), L"Архив успешно создан", L"Сообщение", MB_OK);
		}
	}
}

void decryptFilesEvent()
{
	OPENFILENAMEW file = {};

	file.lStructSize = sizeof(OPENFILENAMEW);
	file.hwndOwner = nullptr;
	file.lpstrFile = fileNameBuffer.data();
	file.lpstrFile[0] = L'\0';
	file.nMaxFile = sizeof(fileNameBuffer);
	file.lpstrFilter = L"Архивные файлы\0*.mfa\0\0";
	file.nFilterIndex = 1;
	file.lpstrFileTitle = nullptr;
	file.nMaxFileTitle = 0;
	file.lpstrInitialDir = nullptr;
	file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	GetOpenFileNameW(&file);

	if (wcslen(file.lpstrFile))
	{
		BinaryFile::decodeBinaryFile(file.lpstrFile);

		MessageBoxW(nullptr, L"Извлечение прошло успешно", L"Сообщение", MB_OK);
	}
}

void changeDirectory(UI::MainWindow& ref)
{
	wstring newPath;
	newPath.resize(256);

	BROWSEINFOW info = {};
	info.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	LPITEMIDLIST test = SHBrowseForFolderW(&info);

	if (test)
	{
		SHGetPathFromIDListW(test, newPath.data());

		while (newPath.back() == '\0')
		{
			newPath.pop_back();
		}

		IMalloc* imalloc = nullptr;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(test);
			imalloc->Release();
		}

		ref.setCurrentDirectory(newPath);
	}
}

void isFolder(vector<wstring>& fullPathFiles)
{
	filesystem::path lastElement = fullPathFiles.back();

	if (filesystem::is_directory(lastElement))
	{
		fullPathFiles.pop_back();

		filesystem::directory_iterator it(lastElement);

		for (auto&& i : it)
		{
			fullPathFiles.push_back(i.path());
			isFolder(fullPathFiles);
		}
	}
}