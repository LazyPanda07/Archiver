#pragma once

inline constexpr __int32 mainWindowWidth = 1024;
inline constexpr __int32 mainWindowHeight = 800;

inline constexpr __int32 controlTopOffset = 50;

inline constexpr __int32 topOffset = 10;
inline constexpr __int32 sideOffset = 10;
inline constexpr __int32 bottomOffset = 10;

inline constexpr __int32 controlButtonsWidth = 160;
inline constexpr __int32 controlButtonsHeight = 50;

inline constexpr __int32 settingsButtonsWidth = 90;
inline constexpr __int32 settingsButtonsHeight = 25;

inline constexpr __int32 settingsStaticWidth = 150;
inline constexpr __int32 settingsStaticHeight = 15;

inline constexpr __int32 settingsEditWidth = 200;
inline constexpr __int32 settingsEditHeight = 20;

inline constexpr __int32 informationMessagesWidth = 300;
inline constexpr __int32 informationMessagesHeight = 15;

enum ButtonEvent : __int32
{
	addFileE = 0xfff,
	deleteFileE,
	chooseFileE,
	encryptFilesE,
	decryptFilesE,
	changeDirectoryE
};

enum Events : __int32
{
	lockMainWindowE = 0x200,
	unlockMainWindowE,
	okSettingsE,
	cancelSettingsE,
	endOfArchivingE
};

inline constexpr __int32 initMainWindowPtr = 0xfffe;
inline constexpr __int32 initListBoxes = 0xffff;
