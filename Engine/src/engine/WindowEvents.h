#pragma once

#include "Window.h"

struct feWindowCloseEvent final
{
	const feWindow* window;
};

struct feWindowKeyEvent final
{
	const feWindow* window;
	int key;
	bool pressed;
};