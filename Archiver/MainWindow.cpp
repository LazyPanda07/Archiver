#include "MainWindow.h"

#include <array>
#include <string>
#include <unordered_map>
#include <stack>
#include <shlobj.h>
#include <thread>

#include "Constants.h"
#include "BinaryFile.h"
#include "UtilityFunctions.h"
#include "ArchiveSettingsWindow.h"
#include "../Utility/SynchronizationHelper.h"

using namespace std;

array<wchar_t, 256> fileNameBuffer{};
unordered_map<wstring, wstring> variants;	//file name - absolute path

LRESULT __stdcall MainWindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

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
void isFolder(vector<pair<wstring, vector<wstring>>>& fullPathFiles);

#pragma endregion

mutex settingsMutex;

namespace UI
{
	MainWindow::MainWindow()
	{
		WNDCLASSEXW wnd = {};
		POINT windowPos = utility::centerCoordinates(mainWindowWidth, mainWindowHeight);
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
			L"������ ����",
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
			L"�������� ����������",
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
			0, controlTopOffset + informationMessagesHeight,
			width / 2, height - controlTopOffset - informationMessagesHeight,
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
			width / 2, controlTopOffset + informationMessagesHeight,
			width / 2, height - controlTopOffset - informationMessagesHeight,
			window,
			HMENU(),
			nullptr,
			nullptr
		);

		availableArea = CreateWindowExW
		(
			NULL,
			L"STATIC",
			L"����� ������ ��� �������������",
			WS_CHILDWINDOW | WS_VISIBLE | SS_CENTER,
			width / 4 - informationMessagesWidth / 2, controlTopOffset,
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
			L"����� ��� �������������",
			WS_CHILDWINDOW | WS_VISIBLE | SS_CENTER,
			width / 2 + width / 4 - informationMessagesWidth / 2, controlTopOffset,
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

		SetWindowPos(availableArea, HWND_BOTTOM, width / 4 - informationMessagesWidth / 2, controlTopOffset, informationMessagesWidth, informationMessagesHeight, SWP_SHOWWINDOW);
		SetWindowPos(addedArea, HWND_BOTTOM, width / 2 + width / 4 - informationMessagesWidth / 2, controlTopOffset, informationMessagesWidth, informationMessagesHeight, SWP_SHOWWINDOW);
		SetWindowPos(availableFiles, HWND_BOTTOM, 0, controlTopOffset + informationMessagesHeight, width / 2, height - controlTopOffset - informationMessagesHeight, SWP_SHOWWINDOW);
		SetWindowPos(addedFiles, HWND_BOTTOM, width / 2, controlTopOffset + informationMessagesHeight, width / 2, height - controlTopOffset - informationMessagesHeight, SWP_SHOWWINDOW);
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

	case endOfArchivingE:


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

		for (__int32 i = elements - 1; i >= 0; i--)
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

		for (__int32 i = elements - 1; i >= 0; i--)
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

		vector<pair<wstring, vector<wstring>>> fullPathFiles;	//files path - files names
		fullPathFiles.reserve(elements);

		for (size_t i = 0; i < elements; i++)
		{
			fullPathFiles.emplace_back(make_pair(L"", vector<wstring>({ variants[files[i]] })));
			isFolder(fullPathFiles);
		}

		if (fullPathFiles.size())
		{
			utility::SynchronizationHelper* synchronization = new utility::SynchronizationHelper();
			wstring* archiveName = new wstring();
			UI::ArchiveSettingsWindow* settings = new UI::ArchiveSettingsWindow(GetParent(addedListBox), *archiveName, synchronization);	//deleted in ArchiveSettingsWindow.cpp

			thread([=]
				{
					UI::ArchiveSettingsWindow* settingsPtr = settings;
					wstring* archiveNamePtr = archiveName;
					utility::SynchronizationHelper* synchronizationPtr = synchronization;

					unique_lock<mutex> lock(settingsMutex);

					synchronizationPtr->sVar.wait(lock, [&settingsPtr] { return settingsPtr->isReady(); });	//wait for getting archive name from ArchiveSettingsWindow

					synchronizationPtr->sState = false;	//init for encode binary function

					BinaryFile::encodeBinaryFile(fullPathFiles, *archiveNamePtr, synchronizationPtr);

					lock.unlock();

					delete archiveNamePtr;
					delete synchronizationPtr;

					MessageBoxW(GetParent(addedListBox), L"����� ������� ������", L"���������", MB_OK);
				}).detach();
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
	file.lpstrFilter = L"�������� �����\0*.mfa\0\0";
	file.nFilterIndex = 1;
	file.lpstrFileTitle = nullptr;
	file.nMaxFileTitle = 0;
	file.lpstrInitialDir = nullptr;
	file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	GetOpenFileNameW(&file);

	if (wcslen(file.lpstrFile))
	{
		BinaryFile::decodeBinaryFile(file.lpstrFile);

		MessageBoxW(nullptr, L"���������� ������ �������", L"���������", MB_OK);
	}
}

void changeDirectory(UI::MainWindow& ref)
{
	static constexpr wchar_t pattern[] = L"������� ����������: ";
	wchar_t title[MAX_PATH];
	const wstring path = ref.getCurrentDirectory().wstring();

	wmemcpy(title, pattern, wcslen(pattern));
	wmemcpy(title + wcslen(pattern), path.data(), path.size());

	title[wcslen(pattern) + path.size()] = 0;

	wstring newPath;
	newPath.resize(256);

	BROWSEINFOW info = {};
	info.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	info.lpfn = [](HWND hwnd, UINT msg, LPARAM, LPARAM data) -> __int32
	{
		if (msg == BFFM_INITIALIZED)
		{
			SendMessageW(hwnd, BFFM_SETSELECTION, true, data);
		}

		return 0;
	};

	info.lParam = reinterpret_cast<LPARAM>(path.data());
	info.lpszTitle = title;

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

void isFolder(vector<pair<wstring, vector<wstring>>>& fullPathFiles)
{
	static vector<wstring> folders;
	const wstring& ref = fullPathFiles.back().second.back();

	filesystem::path lastElement = ref;

	if (filesystem::is_directory(lastElement))
	{
		stack<wstring> subfolders;

		folders.push_back(ref.substr(ref.rfind(L"\\") + 1));
		fullPathFiles.back().second.pop_back();

		filesystem::directory_iterator it(lastElement);

		for (auto&& i : it)
		{
			if (filesystem::is_directory(i))
			{
				subfolders.push(i.path());
			}
			else
			{
				fullPathFiles.back().second.push_back(i.path());
			}
		}

		for (auto&& i : folders)
		{
			fullPathFiles.back().first += i + L"\\";
		}

		fullPathFiles.back().first.pop_back();

		while (subfolders.size())
		{
			fullPathFiles.emplace_back(make_pair(L"", vector<wstring>({ subfolders.top() })));
			subfolders.pop();
			isFolder(fullPathFiles);
		}

		folders.pop_back();
	}
}