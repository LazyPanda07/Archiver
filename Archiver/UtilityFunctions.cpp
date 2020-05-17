#include "UtilityFunctions.h"

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