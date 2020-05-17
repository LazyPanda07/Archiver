#pragma once

#include <Windows.h>

POINT centerCoordinates(HWND window = GetDesktopWindow());

POINT centerCoordinates(LONG width, LONG height, HWND window = GetDesktopWindow());