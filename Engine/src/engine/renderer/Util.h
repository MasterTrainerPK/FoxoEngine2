#pragma once

namespace feRenderUtil
{
	void ClearLoadedFlag();
	unsigned char GetSupportedVersion();

	void Viewport(int x, int y, int w, int h);
	void Clear();
	void InitDefaults(float r, float g, float b, float a);
	void LogOpenGLInfo();
	void SetupDebugLogger();
};