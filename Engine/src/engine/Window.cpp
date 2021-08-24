#include "Window.h"

#include <memory>

#include <glad/gl.h>

#include "Log.h"

static size_t s_Count = 0;

void feWindow::PollEvents()
{
	glfwPollEvents();
}

feLoadProc feWindow::ProcAddress()
{
	return &glfwGetProcAddress;
}

double feWindow::GetTime()
{
	return glfwGetTime();
}

feWindow::feWindow()
{
	Ctor();
}

feWindow::feWindow(const feWindowCreateInfo& info)
{
	Ctor();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, info.contextMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, info.contextMinor);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, info.contextForwardCompat);
	glfwWindowHint(GLFW_OPENGL_PROFILE, info.contextProfileCore ? GLFW_OPENGL_CORE_PROFILE : GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, info.contextDebug);
	glfwWindowHint(GLFW_VISIBLE, info.visible);

	m_Handle = glfwCreateWindow(info.width, info.height, info.title.data(), nullptr, nullptr);

	if (!m_Handle) feLog::Critical("Failed to create window");
	else feLog::Debug("Created window");
}

feWindow::~feWindow()
{
	if (m_Handle)
	{
		feLog::Debug("Destroyed window");
		if(m_Handle == glfwGetCurrentContext()) glfwMakeContextCurrent(nullptr);		
		glfwDestroyWindow(m_Handle);
		m_Handle = nullptr;
	}
	
	--s_Count;

	if (s_Count == 0)
	{
		feLog::Debug("Destroyed GLFW");
		glfwTerminate();
		glfwSetErrorCallback(nullptr);
	}
}

feWindow::feWindow(feWindow&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
}

feWindow& feWindow::operator=(feWindow&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
	return *this;
}

void feWindow::MakeContextCurrent() const
{
	if(m_Handle) glfwMakeContextCurrent(m_Handle);
}

void feWindow::SwapBuffers() const
{
	if (m_Handle) glfwSwapBuffers(m_Handle);
}

bool feWindow::ShouldClose() const
{
	if (!m_Handle) return true;
	return glfwWindowShouldClose(m_Handle);
}

std::pair<int, int> feWindow::GetSize() const
{
	if (!m_Handle) return { 0, 0 };
	int w, h;
	glfwGetWindowSize(m_Handle, &w, &h);

	return { w, h };
}

std::pair<int, int> feWindow::GetViewportSize() const
{
	if (!m_Handle) return { 0, 0 };
	int w, h;
	glfwGetFramebufferSize(m_Handle, &w, &h);

	return { w, h };
}

float feWindow::GetAspect() const
{
	if (!m_Handle) return 0;
	auto [w, h] = GetSize();
	return static_cast<float>(w) / static_cast<float>(h);
}

float feWindow::GetViewportAspect() const
{
	if (!m_Handle) return 0;
	auto [w, h] = GetViewportSize();
	return static_cast<float>(w) / static_cast<float>(h);
}

void feWindow::SetUserPointer(void* ptr) const
{
	if (m_Handle) glfwSetWindowUserPointer(m_Handle, ptr);
}

void feWindow::SetInputMode(int mode, int value) const
{
	if (m_Handle) glfwSetInputMode(m_Handle, mode, value);
}

void feWindow::MakeCenter() const
{
	if (!m_Handle) return;

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	int monitorX, monitorY;
	glfwGetMonitorPos(monitor, &monitorX, &monitorY);

	auto [windowWidth, windowHeight] = GetSize();
	glfwSetWindowPos(m_Handle, monitorX + (mode->width - windowWidth) / 2, monitorY + (mode->height - windowHeight) / 2);
}

GLFWwindow* feWindow::GetHandle() const
{
	return m_Handle;
}

void feWindow::Ctor()
{
	if (s_Count == 0)
	{
		glfwSetErrorCallback([](int error_code, const char* description)
		{
			feLog::Error(fmt::format("GLFW error ", error_code, ": ", description));
		});

		if (!glfwInit()) feLog::Critical("Failed to initialize glfw");
		else feLog::Debug("Initialized GLFW");
	}

	++s_Count;
}

void feContext::Load(const feWindow& window)
{
	window.MakeContextCurrent();
	if (gladLoadGL(feWindow::ProcAddress())) feLog::Trace("Loaded OpenGL functions");
	else feLog::Critical("Failed to load OpenGL functions");
}