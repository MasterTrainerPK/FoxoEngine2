#include <glad/gl.h>

#include <unordered_set>

#include "../engine/Application.h"
#include "../engine/Window.h"
#include "../engine/Log.h"
#include "../engine/renderer/BufferObject.h"
#include "../engine/renderer/VertexArray.h"
#include "../engine/renderer/Shader.h"
#include "../engine/ResourceLoader.h"
#include "../engine/renderer/Util.h"
#include "../engine/renderer/Texture.h"
#include "../engine/renderer/Framebuffer.h"
#include "../engine/math/Transform.h"
#include "../engine/util/Sphere.h"
#include "../engine/Event.h"
#include "../engine/Script.h"
#include "../engine/hierarchy/Scene.h"
#include "../engine/math/Math.h"
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
	info.contextDebug = false;
#else
	info.contextDebug = true;
#endif

	info.visible = visible;

	window = feWindow(info);

	feContext::Load(window);
	feRenderUtil::ClearLoadedFlag();
}

static void LoadKeyMapping(lua_State* L, int index, std::unordered_map<std::string, int>& map)
{
	// Push another reference to the table on top of the stack (so we know
	// where it is, and this function can work for negative, positive and
	// pseudo indices
	lua_pushvalue(L, index);
	// stack now contains: -1 => table
	lua_pushnil(L);
	// stack now contains: -1 => nil; -2 => table
	while (lua_next(L, -2))
	{
		// stack now contains: -1 => value; -2 => key; -3 => table
		// copy the key so that lua_tostring does not modify the original
		lua_pushvalue(L, -2);
		// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
		const char* key = lua_tostring(L, -1);
		int value = (int) lua_tointeger(L, -2);

		map[key] = value;

		//printf("%s => %s\n", key, value);
		// pop value + copy of key, leaving original key
		lua_pop(L, 2);
		// stack now contains: -1 => key; -2 => table
	}
	// stack now contains: -1 => table (when lua_next returns 0 it pops the key
	// but does not push anything.)
	// Pop table
	lua_pop(L, 1);
	// Stack is now the same as it was on entry to this function
}

class Config final
{
public:
	void LoadConfig()
	{
		feScript script;
		script.RunFile("res/scripts/config.lua");

		if (!script.GetGlobalInt("windowWidth", width)) feLog::Error("`windowWidth` should be a number");
		if (!script.GetGlobalInt("windowHeight", height)) feLog::Error("`windowHeight` should be a number");
		if (!script.GetGlobalFloat("mouseSensitivity", sensitivity)) feLog::Error("`mouseSensitivity` should be a number");

		lua_getglobal(script.GetState(), "keyMapping");
		LoadKeyMapping(script.GetState(), -1, m_KeyBindings);
	}

	int width = 800;
	int height = 600;
	float sensitivity = 0.3f;
	std::unordered_map<std::string, int> m_KeyBindings;
};

struct EventWindowMouseLock
{
	bool shouldLock;
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

void MoveCamera(feTransform& transform, const Input& input, float deltaTime, const Config& config)
{
	static bool m_CursorFree = true;

	if (input.IsKeyPressed(GLFW_KEY_ESCAPE))
	{
		m_CursorFree = !m_CursorFree;
		input.GetEventDispatcher().Dispatch<EventWindowMouseLock>(!m_CursorFree);
	}

	if (m_CursorFree) return;

	glm::vec2 mouseDelta = input.GetMouseDelta();

	if (glm::length2(mouseDelta) > 0)
	{
		glm::vec4 axis = transform.GetInverseMatrix() * glm::vec4(0, 1, 0, 0);

		transform.Rotate(glm::radians(mouseDelta.x * -config.sensitivity), glm::vec3(axis));
		transform.Rotate(glm::radians(mouseDelta.y * -config.sensitivity), glm::vec3(1, 0, 0));
	}

	glm::vec4 movementVector = glm::vec4(0, 0, 0, 0);
	float speed = 10.0f;

	if (input.IsKeyDown(config.m_KeyBindings.at("forward"))) --movementVector.z;
	if (input.IsKeyDown(config.m_KeyBindings.at("back"))) ++movementVector.z;
	if (input.IsKeyDown(config.m_KeyBindings.at("left"))) --movementVector.x;
	if (input.IsKeyDown(config.m_KeyBindings.at("right"))) ++movementVector.x;
	if (input.IsKeyDown(config.m_KeyBindings.at("down"))) --movementVector.y;
	if (input.IsKeyDown(config.m_KeyBindings.at("up"))) ++movementVector.y;
	if (input.IsKeyDown(config.m_KeyBindings.at("global_down"))) --movementVector.w;
	if (input.IsKeyDown(config.m_KeyBindings.at("global_up"))) ++movementVector.w;
	if (input.IsKeyDown(config.m_KeyBindings.at("sprint"))) speed = 30.0f;

	if (glm::length2(movementVector) > 0)
	{
		movementVector = glm::normalize(movementVector);
		movementVector *= speed * deltaTime;

		glm::mat4 t = transform.GetMatrix();
		t = glm::translate(t, glm::vec3(movementVector));
		glm::vec4 axis = glm::inverse(t) * glm::vec4(0, 1, 0, 0);
		t = glm::translate(t, glm::vec3(axis) * movementVector.w);

		transform.SetMatrix(t);
	}
}

class feCamera final
{
public:
	glm::mat4 ProjectionMatrix()
	{
		return glm::perspective(glm::radians(fov), aspect, near, far);
	}
public:
	float near = .1f;
	float far = 1000.f;
	float fov = 90.f;
	float aspect = 1.f;
	bool attached = true;
};

class CameraComponent final
{
public:
	feCamera camera;
};

class TransformComponent final
{
public:
	feTransform transform;
};

class MeshFilterComponent final
{
public:
	int a;
};

class Game : public feApplication
{
public:
	virtual ~Game() noexcept = default;

	void UpdateFrameBuffer(int w, int h)
	{
		if (m_FramebufferWidth == w && m_FramebufferHeight == h) return;

		m_FramebufferWidth = w;
		m_FramebufferHeight = h;

		{
			int maxSamples;
			glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

			feLog::Info("GL_MAX_SAMPELS = {}", maxSamples);

			feRenderbufferCreateInfo renderbufferInfo;
			renderbufferInfo.width = m_FramebufferWidth;
			renderbufferInfo.height = m_FramebufferHeight;
			renderbufferInfo.samples = maxSamples;
			renderbufferInfo.internalFormat = GL_RGBA8;
			renderbufferInfo.debugName = "Custom Color Buffer Multisample";

			m_FramebufferColorBufferMultisample = renderbufferInfo;

			renderbufferInfo.width = m_FramebufferWidth;
			renderbufferInfo.height = m_FramebufferHeight;
			renderbufferInfo.samples = maxSamples;
			renderbufferInfo.internalFormat = GL_DEPTH_COMPONENT24;
			renderbufferInfo.debugName = "Custom Depth Buffer Multisample";

			m_FramebufferDepthBufferMultisample = renderbufferInfo;

			feFramebufferCreateInfoRenderbufferAttachmentInfo attachmentInfos[2];
			attachmentInfos[0].attachment = GL_COLOR_ATTACHMENT0;
			attachmentInfos[0].renderbuffer = &m_FramebufferColorBufferMultisample;
			attachmentInfos[1].attachment = GL_DEPTH_ATTACHMENT;
			attachmentInfos[1].renderbuffer = &m_FramebufferDepthBufferMultisample;

			feFramebufferCreateInfo framebufferInfo;
			framebufferInfo.renderbufferAttachments = attachmentInfos;
			framebufferInfo.renderbufferAttachmentCount = 2;
			framebufferInfo.debugName = "Custom Framebuffer Multisample";

			m_FramebufferMultisample = framebufferInfo;
		}

		// Create renderbffers
		{
			feRenderbufferCreateInfo renderbufferInfo;
			renderbufferInfo.width = m_FramebufferWidth;
			renderbufferInfo.height = m_FramebufferHeight;
			renderbufferInfo.internalFormat = GL_SRGB8_ALPHA8;
			renderbufferInfo.debugName = "Custom Color Buffer";

			m_FramebufferColorBuffer = renderbufferInfo;

			renderbufferInfo.width = m_FramebufferWidth;
			renderbufferInfo.height = m_FramebufferHeight;
			renderbufferInfo.internalFormat = GL_DEPTH_COMPONENT24;
			renderbufferInfo.debugName = "Custom Depth Buffer";

			m_FramebufferDepthBuffer = renderbufferInfo;

			feFramebufferCreateInfoRenderbufferAttachmentInfo attachmentInfos[2];
			attachmentInfos[0].attachment = GL_COLOR_ATTACHMENT0;
			attachmentInfos[0].renderbuffer = &m_FramebufferColorBuffer;
			attachmentInfos[1].attachment = GL_DEPTH_ATTACHMENT;
			attachmentInfos[1].renderbuffer = &m_FramebufferDepthBuffer;

			feFramebufferCreateInfo framebufferInfo;
			framebufferInfo.renderbufferAttachments = attachmentInfos;
			framebufferInfo.renderbufferAttachmentCount = 2;
			framebufferInfo.debugName = "Custom Framebuffer";

			m_Framebuffer = framebufferInfo;
		}
	}

	virtual void Init() override
	{
		m_Config.LoadConfig();

		// Minimum version required is OpenGL 4.0
		unsigned char version = 40;

		{
			feWindow window;
			MakeWindow(window, m_Config.width, m_Config.height, version, false, false);
			version = feRenderUtil::GetSupportedVersion();
		}

		MakeWindow(m_Window, m_Config.width, m_Config.height, version, true, true);

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

		Sphere sphere = Sphere(1, 36, 18, true);

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

		{
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
		}

		{
			std::string vertSrc = feResourceLoader::LoadTextFile("res/shaders/simple.vert").value();
			std::string fragSrc = feResourceLoader::LoadTextFile("res/shaders/simple.frag").value();

			feShader shaders[2];

			feShaderCreateInfo shaderInfo;
			shaderInfo.type = GL_VERTEX_SHADER;
			std::string_view view = vertSrc;
			shaderInfo.sources = &view;
			shaderInfo.sourceCount = 1;
			shaderInfo.debugName = "res/shaders/simple.vert";

			shaders[0] = feShader(shaderInfo);

			shaderInfo.type = GL_FRAGMENT_SHADER;
			shaderInfo.debugName = "res/shaders/simple.frag";
			view = fragSrc;

			shaders[1] = feShader(shaderInfo);

			feProgramCreateInfo programInfo;
			programInfo.shaders = shaders;
			programInfo.shaderCount = 2;
			programInfo.debugName = "Main Program";

			m_Program = programInfo;
			m_Program.Bind();
			m_Program.Uniform1i("u_Albedo", 0);
		}

		{
			feImage image("res/textures/grass.png", 4);

			feTextureCreateInfo textureInfo;
			textureInfo.target = GL_TEXTURE_2D;
			textureInfo.width = image.GetWidth();
			textureInfo.height = image.GetHeight();
			textureInfo.pixels = image.GetPixels();
			textureInfo.format = GL_RGBA;
			textureInfo.internalFormat = GL_SRGB8_ALPHA8;
			textureInfo.type = GL_UNSIGNED_BYTE;
			textureInfo.debugName = "res/textures/grass.png";
			textureInfo.mipmaps = true;
			textureInfo.filterMin = GL_LINEAR_MIPMAP_LINEAR;
			textureInfo.filterMag = GL_LINEAR;
			textureInfo.wrap = GL_REPEAT;
			m_Texture = textureInfo;
		}

		m_EventDispatcher.Subscribe(this, &Game::OnWindowClose);
		m_EventDispatcher.Subscribe(this, &Game::OnWindowCursorModeChange);
		m_Input.Set(m_EventDispatcher);

		// Loads the feApi
		m_Script.RunFile("res/scripts/api.lua");

		lua_register(m_Script.GetState(), "feApiCreateEntity", [](lua_State* L)
		{
			Game* game = static_cast<Game*>(lua_touserdata(L, 1));

			feEntity entity = game->m_Scene.CreateEntity();
			feEntity* luaEntity = static_cast<feEntity*>(lua_newuserdata(L, sizeof(feEntity)));
			*luaEntity = entity;

			return 1;
		});

		lua_register(m_Script.GetState(), "feApiCreateComponent", [](lua_State* L)
		{
			feEntity& entity = *static_cast<feEntity*>(lua_touserdata(L, 1));
			std::string_view type = lua_tostring(L, 2);

			if (type == "Transform")
			{
				auto& ref = entity.CreateComponent<TransformComponent>();
				lua_pushlightuserdata(L, &ref);
			}
			else if (type == "MeshFilterComponent")
			{
				auto& ref = entity.CreateComponent<MeshFilterComponent>();
				lua_pushlightuserdata(L, &ref);
			}
			else
			{
				lua_pushnil(L);
			}

			return 1;
		});

		lua_register(m_Script.GetState(), "feApiComponentTransformSet", [](lua_State* L)
		{
			TransformComponent* ptr = static_cast<TransformComponent*>(lua_touserdata(L, 1));

			auto readFloat3 = [](lua_State* L)
			{
				lua_pushstring(L, "x");
				lua_gettable(L, -2);

				lua_pushstring(L, "y");
				lua_gettable(L, -3);

				lua_pushstring(L, "z");
				lua_gettable(L, -4);

				glm::vec3 pos;
				pos.x = lua_tonumber(L, -3);
				pos.y = lua_tonumber(L, -2);
				pos.z = lua_tonumber(L, -1);

				lua_pop(L, 3);

				return pos;
			};

			auto readFloat4 = [](lua_State* L)
			{
				lua_pushstring(L, "x");
				lua_gettable(L, -2);

				lua_pushstring(L, "y");
				lua_gettable(L, -3);

				lua_pushstring(L, "z");
				lua_gettable(L, -4);

				lua_pushstring(L, "w");
				lua_gettable(L, -5);

				glm::vec4 pos;
				pos.x = lua_tonumber(L, -4);
				pos.y = lua_tonumber(L, -3);
				pos.z = lua_tonumber(L, -2);
				pos.w = lua_tonumber(L, -1);

				lua_pop(L, 4);

				return pos;
			};

			lua_pushstring(L, "pos");
			lua_gettable(L, -2);
			glm::vec3 pos = readFloat3(L);
			lua_pop(L, 1);

			lua_pushstring(L, "quat");
			lua_gettable(L, -2);
			glm::vec4 quat = readFloat4(L);
			lua_pop(L, 1);

			lua_pushstring(L, "sca");
			lua_gettable(L, -2);
			glm::vec3 sca = readFloat3(L);
			lua_pop(L, 1);

			ptr->transform.pos = pos;
			ptr->transform.quat = glm::quat(quat.x, quat.y, quat.z, quat.w);
			ptr->transform.sca = sca;

			return 0;
		});
		
		// Load the script
		m_Script.RunFile("res/scripts/game.lua");

		// Call global Init function from the script
		lua_getglobal(m_Script.GetState(), "Init");
		if (lua_isfunction(m_Script.GetState(), -1))
		{
			lua_pushlightuserdata(m_Script.GetState(), this);
			if(lua_pcall(m_Script.GetState(), 1, 0, 0)) feLog::Error(lua_tostring(m_Script.GetState(), -1));
		}

		// Create camera
		{
			feEntity entity = m_Scene.CreateEntity();
			entity.CreateComponent<TransformComponent>();
			entity.CreateComponent<CameraComponent>();
		}
	}

	virtual void Destroy() override
	{
		// Call global Init function from the script
		lua_getglobal(m_Script.GetState(), "Destroy");
		if (lua_isfunction(m_Script.GetState(), -1))
		{
			lua_pushlightuserdata(m_Script.GetState(), this);
			if (lua_pcall(m_Script.GetState(), 1, 0, 0)) feLog::Error(lua_tostring(m_Script.GetState(), -1));
		}

		m_EventDispatcher.Unsubscribe(this);
		m_Input.Unset();
	}

	void OnWindowClose(const feEventWindowClose& event)
	{
		Stop();
	}

	void OnWindowCursorModeChange(const EventWindowMouseLock& event)
	{
		glfwSetInputMode(m_Window.GetHandle(), GLFW_CURSOR, event.shouldLock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
	
	virtual void Update() override
	{
		m_Input.Update();

		{
			auto view = m_Scene.m_Registry.view<TransformComponent, MeshFilterComponent>();

			for (auto id : view)
			{
				auto& [transform, meshFilter] = view.get(id);

				transform.transform.Rotate(glm::radians((float) GetDeltaTime() * 30), glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f)));
			}
		}

		// Call update function from script
		lua_getglobal(m_Script.GetState(), "Update");
		if (lua_isfunction(m_Script.GetState(), -1))
		{
			lua_pushlightuserdata(m_Script.GetState(), this);
			if (lua_pcall(m_Script.GetState(), 1, 0, 0)) feLog::Error(lua_tostring(m_Script.GetState(), -1));
		}

		feTransform* cameraTransform = nullptr;
		feCamera* cameraCamera = nullptr;

		// Look for any entities with both a transform and a camera
		{
			auto view = m_Scene.m_Registry.view<TransformComponent, CameraComponent>();

			for (auto id : view)
			{
				auto& [transform, camera] = view.get(id);

				MoveCamera(transform.transform, m_Input, (float) GetDeltaTime(), m_Config);

				if (!cameraTransform && !cameraCamera)
				{
					cameraTransform = &transform.transform;
					cameraCamera = &camera.camera;
				}
			}
		}

		// Only render if an entity has both a camera and a transform
		if (cameraTransform && cameraCamera)
		{
			// Don't render if the window is iconified
			auto [w, h] = m_Window.GetViewportSize();
			if (w == 0 || h == 0) return;

			UpdateFrameBuffer(w, h);

			

			m_FramebufferMultisample.Bind();

			// feRenderUtil::Viewport(0, 0, w, h);
			feRenderUtil::Viewport(0, 0, m_FramebufferWidth, m_FramebufferHeight);
			feRenderUtil::Clear();

			if (cameraCamera->attached) cameraCamera->aspect = m_Window.GetAspect();
			glm::mat4 proj = cameraCamera->ProjectionMatrix();

			m_Texture.Bind(0);
			m_Program.Bind();
			m_Program.UniformMat4f("u_View", cameraTransform->GetInverseMatrix());
			m_Program.UniformMat4f("u_Proj", proj);
			m_Vao.Bind();

			{
				auto view = m_Scene.m_Registry.view<TransformComponent, MeshFilterComponent>();

				for (auto id : view)
				{
					auto& [transform, meshFilter] = view.get(id);

					m_Program.UniformMat4f("u_Model", transform.transform.GetMatrix());
					m_Vao.Draw();					
				}
			}

			{
				feFramebufferBlitInfo info;

				feFramebuffer defaultFbo = feFramebuffer();

				info.dest = &m_Framebuffer;

				info.srcX0 = 0;
				info.srcX1 = m_FramebufferWidth;
				info.srcY0 = 0;
				info.srcY1 = m_FramebufferHeight;

				info.dstX0 = 0;
				info.dstX1 = m_FramebufferWidth;
				info.dstY0 = 0;
				info.dstY1 = m_FramebufferHeight;

				info.mask = GL_COLOR_BUFFER_BIT;
				info.filter = GL_NEAREST;

				
				glEnable(GL_FRAMEBUFFER_SRGB);
				m_FramebufferMultisample.Blit(info);
				glDisable(GL_FRAMEBUFFER_SRGB);

				info.dest = &defaultFbo;
				info.dstX0 = 0;
				info.dstX1 = w;
				info.dstY0 = 0;
				info.dstY1 = h;

				m_Framebuffer.Blit(info);
			}
		}

		m_Window.SwapBuffers();
	}

	virtual double GetTime() override
	{
		return feWindow::GetTime();
	}

	feEventDispatcher m_EventDispatcher;
	feWindow m_Window;
	feScene m_Scene;
private:

	feVertexArray m_Vao;
	feBufferObject m_Vbo;
	feBufferObject m_Ibo;
	feProgram m_Program;
	feTexture m_Texture;
	feScript m_Script;
	
	Input m_Input;
	Config m_Config;

	// Fbo info
	feFramebuffer m_Framebuffer;
	feRenderbuffer m_FramebufferColorBuffer;
	feRenderbuffer m_FramebufferDepthBuffer;

	feFramebuffer m_FramebufferMultisample;
	feRenderbuffer m_FramebufferColorBufferMultisample;
	feRenderbuffer m_FramebufferDepthBufferMultisample;
	int m_FramebufferWidth = -1;
	int m_FramebufferHeight = -1;
};

feApplication* feApplication::CreateInstance()
{
	return new Game;
}

void feApplication::DeleteInstance(feApplication* application)
{
	delete application;
}