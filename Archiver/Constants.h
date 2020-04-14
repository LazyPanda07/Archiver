#pragma once

constexpr int mainWindowWidth = 1024;
constexpr int mainWindowHeight = 800;

constexpr int topOffset = 50;

constexpr int controlButtonsWidth = 150;
constexpr int controlButtonsHeight = 50;

constexpr int informationMessagesWidth = 200;
constexpr int informationMessagesHeight = 15;

enum ButtonEvent : __int32
{
	addFileE=0xfff,
	deleteFileE,
	chooseFileE,
	encryptFilesE,
	decryptFilesE
};

constexpr int initMainWindowPtr = 0xfffe;
constexpr int initListBoxes = 0xffff;
