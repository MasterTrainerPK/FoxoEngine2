#include <glad/gl.h>

#include <unordered_set>

#include <lua.hpp>

#include "../engine/Application.h"
#include "../engine/Window.h"
#include "../engine/Log.h"
#include "../engine/renderer/BufferObject.h"
#include "../engine/renderer/VertexArray.h"
#include "../engine/renderer/Shader.h"
#include "../engine/ResourceLoader.h"
#include "../engine/renderer/Util.h"
#include "../engine/math/Transform.h"
#include "../engine/util/Sphere.h"
#include "../engine/Event.h"
#include "WindowEvents.h"

static void MakeWindow(feWindow& window, int width, int height, unsigned char version, bool useNewStuff, bool visible)
{
	feWindowCreateInfo info;
	info.width = width;
	info.height = height;
	info.title = "3D Engine";
	info.contextMajor = version / 10;
	info.contextMinor = version % 10;
	info.contextForwardCompat = useNewStuff;
	info.contextProfileCore = useNewStuff;
#if defined(FE_CONF_DIST)
	info.contextDebug = true;
#else
	info.contextDebug = false;
#endif

	info.visible = visible;

	window = feWindow(info);

	feContext::Load(window);
	feRenderUtil::ClearLoadedFlag();
}

class ScriptState final
{
public:
	ScriptState()
	{
		L = luaL_newstate();
		luaL_openlibs(L);
	}

	void Run(const char* file)
	{
		if (luaL_dofile(L, file))
		{
			feLog::Error(lua_tostring(L, -1));
		}
	}

	~ScriptState()
	{
		lua_close(L);
	}

	lua_State* L;
};

class Config final
{
public:
	Config()
	{
		ScriptState state;
		state.Run("res/scripts/config.lua");

		lua_getglobal(state.L, "windowWidth");
		lua_getglobal(state.L, "windowHeight");
		if (!lua_isnumber(state.L, -2)) feLog::Error("Width should be a number");
		if (!lua_isnumber(state.L, -1)) feLog::Error("Height should be a number");
		width = (int) lua_tointeger(state.L, -2);
		height = (int) lua_tointeger(state.L, -1);

		lua_pop(state.L, 1);
	}

	int width = 0;
	int height = 0;
};

struct WindowEventInputMode
{
	int mode;
};

class Input final
{
public:
	void OnKey(const feEventWindowKey& event)
	{
		if(event.pressed) m_Keys.insert(event.key);
		else m_Keys.erase(event.key);
	}

	void OnMouseMove(const feEventWindowMouseMove& event)
	{
		m_Mouse.x = event.x;
		m_Mouse.y = event.y;
	}

	Input() = default;

	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

public:
	void Set(feEventDispatcher& dispatcher)
	{
		m_Dispatcher = &dispatcher;
		dispatcher.Subscribe(this, &Input::OnKey);
		dispatcher.Subscribe(this, &Input::OnMouseMove);
	}

	void Unset()
	{
		m_Dispatcher->Unsubscribe(this);
		m_Dispatcher = nullptr;
	}

	void Update()
	{
		m_KeysLast = m_Keys;
		m_MouseLast = m_Mouse;
		feWindow::PollEvents();
	}

	bool IsKeyDown(int key) const
	{
		return m_Keys.find(key) != m_Keys.end();
	}

	bool IsKeyPressed(int key) const
	{
		return IsKeyDown(key) && m_KeysLast.find(key) == m_KeysLast.end();
	}

	glm::vec2 GetMouseDelta() const
	{
		return m_Mouse - m_MouseLast;
	}

	const feEventDispatcher& GetEventDispatcher() const
	{
		return *m_Dispatcher;
	}
private:
	std::unordered_set<int> m_Keys;
	std::unordered_set<int> m_KeysLast;
	glm::vec2 m_Mouse = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_MouseLast = glm::vec2(0.0f, 0.0f);

	feEventDispatcher* m_Dispatcher = nullptr;
};

class Camera final
{
public:
	void Move(const Input& input, float deltaTime)
	{
		if (input.IsKeyPressed(GLFW_KEY_ESCAPE))
		{
			m_Locked = !m_Locked;

			if (m_Locked) input.GetEventDispatcher().Dispatch<WindowEventInputMode>(GLFW_CURSOR_NORMAL);
			else input.GetEventDispatcher().Dispatch<WindowEventInputMode>(GLFW_CURSOR_DISABLED);
		}

		if (m_Locked) return;

		constexpr float mouseSensitivity = 0.3f;

		glm::vec2 mouseDelta = input.GetMouseDelta();
		
		if (glm::length2(mouseDelta) > 0)
		{
			glm::vec4 axis = glm::inverse(m_Transform.GetMatrix()) * glm::vec4(0, 1, 0, 0);

			m_Transform.Rotate(glm::radians(mouseDelta.x * -mouseSensitivity), glm::vec3(axis));
			m_Transform.Rotate(glm::radians(mouseDelta.y * -mouseSensitivity), glm::vec3(1, 0, 0));
		}

		glm::vec4 movementVector = glm::vec4(0, 0, 0, 0);
		float speed = 10.0f;

		if (input.IsKeyDown(GLFW_KEY_W)) --movementVector.z;
		if (input.IsKeyDown(GLFW_KEY_S)) ++movementVector.z;
		if (input.IsKeyDown(GLFW_KEY_A)) --movementVector.x;
		if (input.IsKeyDown(GLFW_KEY_D)) ++movementVector.x;
		if (input.IsKeyDown(GLFW_KEY_Q)) --movementVector.y;
		if (input.IsKeyDown(GLFW_KEY_E)) ++movementVector.y;
		if (input.IsKeyDown(GLFW_KEY_LEFT_SHIFT)) --movementVector.w;
		if (input.IsKeyDown(GLFW_KEY_SPACE)) ++movementVector.w;
		if (input.IsKeyDown(GLFW_KEY_LEFT_CONTROL)) speed = 30.0f;

		if (glm::length2(movementVector) > 0)
		{
			movementVector = glm::normalize(movementVector);
			movementVector *= speed * deltaTime;

			glm::mat4 transform = m_Transform.GetMatrix();
			transform = glm::translate(transform, glm::vec3(movementVector));
			glm::vec4 axis = glm::inverse(transform) * glm::vec4(0, 1, 0, 0);
			transform = glm::translate(transform, glm::vec3(axis) * movementVector.w);

			m_Transform.SetMatrix(transform);
		}
	}
public:
	feTransform m_Transform;
	bool m_Locked = true;
};

class Game : public feApplication
{
public:
	virtual ~Game() noexcept = default;

	virtual void Init() override
	{
		Config config = Config();

		// Minimum version required is OpenGL 4.0
		unsigned char version = 40;

		{
			feWindow window;
			MakeWindow(window, config.width, config.height, version, false, false);
			version = feRenderUtil::GetSupportedVersion();
		}

		MakeWindow(m_Window, config.width, config.height, version, true, true);

		m_Window.SetUserPointer(this);
		glfwSetWindowCloseCallback(m_Window.GetHandle(), [](GLFWwindow* window)
		{
			Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
			game->m_EventDispatcher.Dispatch<feEventWindowClose>(&game->m_Window);
		});

		glfwSetKeyCallback(m_Window.GetHandle(), [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

			// Ignore repeat codes
			if (action == GLFW_REPEAT) return;

			game->m_EventDispatcher.Dispatch<feEventWindowKey>(&game->m_Window, key, action == GLFW_PRESS);
		});


		glfwSetCursorPosCallback(m_Window.GetHandle(), [](GLFWwindow* window, double x, double y)
		{
			Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

			game->m_EventDispatcher.Dispatch<feEventWindowMouseMove>(&game->m_Window, float(x), float(y));
		});

		feRenderUtil::LogOpenGLInfo();
		feRenderUtil::InitDefaults(0.7f, 0.8f, 0.9f, 1.0f);
		feRenderUtil::SetupDebugLogger();

		Sphere sphere = Sphere(1, 36, 18, false);

		{
			feBufferObjectCreateInfo info;
			info.target = GL_ARRAY_BUFFER;
			info.data = sphere.getInterleavedVertices();
			info.size = sphere.getInterleavedVertexSize();
			info.debugName = "Sphere VBO";

			m_Vbo = info;
		}

		{
			feBufferObjectCreateInfo info;
			info.target = GL_ELEMENT_ARRAY_BUFFER;
			info.data = sphere.getIndices();
			info.size = sphere.getIndexSize();
			info.debugName = "Sphere indices";

			m_Ibo = info;
		}
		
		feVertexArrayCreateInfoBufferObjectInfo vaoVboInfo;
		vaoVboInfo.buffer = &m_Vbo;
		vaoVboInfo.stride = 8 * sizeof(float);

		feVertexArrayCreateInfoAttributeInfo attributeInfos[3];

		attributeInfos[0].buffer = 0;
		attributeInfos[0].offset = 0 * sizeof(float);
		attributeInfos[0].size = 3;
		attributeInfos[0].type = GL_FLOAT;

		attributeInfos[1].buffer = 0;
		attributeInfos[1].offset = 3 * sizeof(float);
		attributeInfos[1].size = 3;
		attributeInfos[1].type = GL_FLOAT;

		attributeInfos[2].buffer = 0;
		attributeInfos[2].offset = 5 * sizeof(float);
		attributeInfos[2].size = 2;
		attributeInfos[2].type = GL_FLOAT;

		feVertexArrayCreateInfo vaoInfo;
		vaoInfo.attributeInfos = attributeInfos;
		vaoInfo.attributeInfoCount = 3;
		vaoInfo.count = sphere.getIndexCount();
		vaoInfo.mode = GL_TRIANGLES;
		vaoInfo.vertexBufferInfos = &vaoVboInfo;
		vaoInfo.vertexBufferInfoCount = 1;
		vaoInfo.indexBuffer = &m_Ibo;
		vaoInfo.debugName = "Sphere vao";

		m_Vao = vaoInfo;

		std::string vertSrc = feResourceLoader::LoadTextFile("res/shaders/simple.vert").value();
		std::string fragSrc = feResourceLoader::LoadTextFile("res/shaders/simple.frag").value();

		feShader shaders[2];

		feShaderCreateInfo shaderInfo;
		shaderInfo.type = GL_VERTEX_SHADER;
		std::string_view view = vertSrc;
		shaderInfo.sources = &view;
		shaderInfo.sourceCount = 1;
		shaderInfo.debugName = "Shader Vertex";

		shaders[0] = feShader(shaderInfo);

		shaderInfo.type = GL_FRAGMENT_SHADER;
		shaderInfo.debugName = "Shader Fragment";
		view = fragSrc;

		shaders[1] = feShader(shaderInfo);

		feProgramCreateInfo programInfo;
		programInfo.shaders = shaders;
		programInfo.shaderCount = 2;
		programInfo.debugName = "Main Program";

		m_Program = programInfo;

		m_Script.Run("res/scripts/game.lua");

		m_EventDispatcher.Subscribe(this, &Game::OnWindowClose);
		m_EventDispatcher.Subscribe(this, &Game::OnWindowCursorModeChange);
		m_Input.Set(m_EventDispatcher);
	}

	virtual void Destroy() override
	{
		m_EventDispatcher.Unsubscribe(this);
		m_Input.Unset();
	}

	void OnWindowClose(const feEventWindowClose& event)
	{
		Stop();
	}

	void OnWindowCursorModeChange(const WindowEventInputMode& event)
	{
		glfwSetInputMode(m_Window.GetHandle(), GLFW_CURSOR, event.mode);
	}
	
	virtual void Update() override
	{
		m_Input.Update();

		auto [w, h] = m_Window.GetViewportSize();

		// Don't render if the window is iconified
		if (w == 0 || h == 0) return;

		m_Camera.Move(m_Input, (float) GetDeltaTime());

		feRenderUtil::Viewport(0, 0, w, h);
		feRenderUtil::Clear();

		glm::mat4 proj = glm::perspective(glm::radians(80.f), m_Window.GetAspect(), 0.1f, 100.0f);

		m_Transform.pos = { 0, 0, -3 };
		m_Transform.Rotate(glm::radians((float) GetDeltaTime() * 30), glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f)));

		m_Program.Bind();
		m_Program.Uniform3f("u_Color", { 1.0f, 0.5f, 0.0f });
		m_Program.UniformMat4f("u_Model", m_Transform.GetMatrix());
		m_Program.UniformMat4f("u_View", glm::inverse(m_Camera.m_Transform.GetMatrix()));
		m_Program.UniformMat4f("u_Proj", proj);
		m_Vao.Bind();
		m_Vao.Draw();

		m_Window.SwapBuffers();
	}

	virtual double GetTime() override
	{
		return feWindow::GetTime();
	}

	feEventDispatcher m_EventDispatcher;
	feWindow m_Window;
private:

	feVertexArray m_Vao;
	feBufferObject m_Vbo;
	feBufferObject m_Ibo;
	feProgram m_Program;

	ScriptState m_Script;

	feTransform m_Transform;

	Input m_Input;
	Camera m_Camera;
};

feApplication* feApplication::CreateInstance()
{
	return new Game;
}

void feApplication::DeleteInstance(feApplication* application)
{
	delete application;
}