#pragma once

#include "Window.h"

struct feEventWindowClose final
{
	const feWindow* window;
};

struct feEventWindowKey final
{
	const feWindow* window;
	int key;
	bool pressed;
};

struct feEventWindowMouseMove final
{
	const feWindow* window;
	float x, y;
};