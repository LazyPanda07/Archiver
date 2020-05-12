#pragma once

inline constexpr int mainWindowWidth = 1024;
inline constexpr int mainWindowHeight = 800;

inline constexpr int topOffset = 50;

inline constexpr int controlButtonsWidth = 150;
inline constexpr int controlButtonsHeight = 50;

inline constexpr int informationMessagesWidth = 300;
inline constexpr int informationMessagesHeight = 15;

enum ButtonEvent : __int32
{
	addFileE=0xfff,
	deleteFileE,
	chooseFileE,
	encryptFilesE,
	decryptFilesE
};

inline constexpr int initMainWindowPtr = 0xfffe;
inline constexpr int initListBoxes = 0xffff;
