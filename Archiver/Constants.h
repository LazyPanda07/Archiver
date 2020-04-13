#pragma once

constexpr int mainWindowWidth = 800;
constexpr int mainWindowHeight = 600;

constexpr int topOffset = 50;

constexpr int controlButtonsWidth = 150;
constexpr int controlButtonsHeight = 50;

constexpr int informationMessagesWidth = 300;
constexpr int informationMessagesHeight = 15;

enum ButtonEvent : __int32
{
	addFileE=0xfff,
	deleteFileE,
	chooseFileE
};

constexpr int initListBoxes = 0xffff;