#pragma once

#include "../Window.h"

namespace feRenderUtil
{
	void ClearLoadedFlag();
	unsigned char GetSupportedVersion();

	void Viewport(int x, int y, int w, int h);
	void Viewport(const feWindow& window);
	void Clear();
	void InitDefaults();
	void LogOpenGLInfo();
	void SetupDebugLogger();
};