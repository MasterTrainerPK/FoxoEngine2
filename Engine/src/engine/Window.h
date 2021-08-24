#pragma once

#include <utility>
#include <string_view>

#include <GLFW/glfw3.h>

struct feWindowCreateInfo final
{
	int width = 800;
	int height = 600;
	std::string_view title = "FoxoEngine";
	int contextMajor = 1;
	int contextMinor = 0;
	bool contextForwardCompat = false;
	bool contextProfileCore = false;
	bool contextDebug = false;
	bool visible = true;
};

typedef void (*feProc)(void);
typedef feProc(*feLoadProc)(const char* procname);

class feWindow final
{
public:
	static void PollEvents();
	[[nodiscard]] static feLoadProc ProcAddress();
	[[nodiscard]] static double GetTime();
public:
	feWindow();
	feWindow(const feWindowCreateInfo& info);
	~feWindow() noexcept;

	feWindow(const feWindow&) = delete;
	feWindow& operator=(const feWindow&) = delete;

	feWindow(feWindow&& other) noexcept;
	feWindow& operator=(feWindow&& other) noexcept;

	void MakeContextCurrent() const;
	void SwapBuffers() const;
	[[nodiscard]] bool ShouldClose() const;
	[[nodiscard]] std::pair<int, int> GetSize() const;
	[[nodiscard]] float GetAspect() const;
	[[nodiscard]] std::pair<int, int> GetViewportSize() const;
	[[nodiscard]] float GetViewportAspect() const;
	void SetUserPointer(void* ptr) const;
	void SetInputMode(int mode, int value) const;
	void MakeCenter() const;
	[[nodiscard]] GLFWwindow* GetHandle() const;
private:
	void Ctor();
private:
	GLFWwindow* m_Handle = nullptr;
};

class feContext final
{
public:
	static void Load(const feWindow& window);
};