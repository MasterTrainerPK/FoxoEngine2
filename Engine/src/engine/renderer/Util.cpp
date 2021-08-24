#include "Util.h"

#include <optional>
#include <unordered_map>
#include <string>

#include <glad/gl.h>

#include "../Log.h"

#define FE_EXPAND_GL(x) { x, #x }

static struct feRenderUtilLoadedFlags final
{
	std::optional<unsigned char> version;
} s_Flags;

static std::string GetOpenGLString(unsigned int value)
{
	const std::unordered_map<unsigned int, const char*> data =
	{
		{ GL_DEBUG_SOURCE_API, "API" },
		{ GL_DEBUG_SOURCE_WINDOW_SYSTEM, "WINDOW_SYSTEM" },
		{ GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER_COMPILER" },
		{ GL_DEBUG_SOURCE_THIRD_PARTY, "THIRD_PARTY" },
		{ GL_DEBUG_SOURCE_APPLICATION, "APPLICATION" },
		{ GL_DEBUG_SOURCE_OTHER, "OTHER" },
		{ GL_DEBUG_TYPE_ERROR, "ERROR" },
		{ GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR" },
		{ GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOR" },
		{ GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY" },
		{ GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE" },
		{ GL_DEBUG_TYPE_MARKER, "MARKER" },
		{ GL_DEBUG_TYPE_PUSH_GROUP, "PUSH_GROUP" },
		{ GL_DEBUG_TYPE_POP_GROUP, "POP_GROUP" },
		{ GL_DEBUG_TYPE_OTHER, "OTHER" },
		{ GL_DEBUG_SEVERITY_NOTIFICATION, "NOTIFICATION" },
		{ GL_DEBUG_SEVERITY_LOW, "LOW" },
		{ GL_DEBUG_SEVERITY_MEDIUM, "MEDIUM" },
		{ GL_DEBUG_SEVERITY_HIGH, "HIGH" },
		FE_EXPAND_GL(GL_FRAMEBUFFER_UNDEFINED),
		FE_EXPAND_GL(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT),
		FE_EXPAND_GL(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT),
		FE_EXPAND_GL(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER),
		FE_EXPAND_GL(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER),
		FE_EXPAND_GL(GL_FRAMEBUFFER_UNSUPPORTED),
		FE_EXPAND_GL(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE),
		FE_EXPAND_GL(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
	};

	auto result = data.find(value);
	if (result != data.end()) return std::string(result->second);
	return std::to_string(value);
};

static void OpenGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	feLog::Error("{}, {}, {}, {}: {}", GetOpenGLString(source), GetOpenGLString(type), GetOpenGLString(severity), GetOpenGLString(id), message);
	if (severity == GL_DEBUG_SEVERITY_HIGH) feLog::Break();
}

namespace feRenderUtil
{
	void ClearLoadedFlag()
	{
		s_Flags = feRenderUtilLoadedFlags();
	}

	unsigned char GetSupportedVersion()
	{
		if (s_Flags.version) return s_Flags.version.value();

		unsigned char version = 0;
		const unsigned char* versionStr = glGetString(GL_VERSION);
		version += (versionStr[0] - '0') * 10;
		version += versionStr[2] - '0';

		s_Flags.version = version;
		return version;
	}

	void Viewport(int x, int y, int w, int h)
	{
		glViewport(x, y, w, h);
	}

	void Viewport(const feWindow& window)
	{
		auto [w, h] = window.GetSize();
		Viewport(0, 0, w, h);
	}

	void Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void InitDefaults()
	{
		glClearColor(1, 0, 0, 0);
		glClearDepth(1);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
	}

	void LogOpenGLInfo()
	{
		feLog::Info("GL_RENDERER = {}", glGetString(GL_RENDERER));
		feLog::Info("GL_VENDOR = {}", glGetString(GL_VENDOR));
		feLog::Info("GL_VERSION = {}", glGetString(GL_VERSION));
		feLog::Info("GL_SHADING_LANGUAGE_VERSION = {}", glGetString(GL_SHADING_LANGUAGE_VERSION));

		GLint numExt;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);

		for (unsigned int i = 0; i < numExt; ++i)
		{
			const GLubyte* ext = glGetStringi(GL_EXTENSIONS, i);
			feLog::Debug((const char*) ext);
		}
	}

	void SetupDebugLogger()
	{
		if (GetSupportedVersion() < 43)
		{
			feLog::Warn("Debug logging is not supported");
			return;
		}

		int flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			feLog::Debug("OpenGL debug context is being used");

			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(&OpenGLMessageCallback, nullptr);

			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		}
	}
};