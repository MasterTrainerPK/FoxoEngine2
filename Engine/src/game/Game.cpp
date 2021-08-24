#include <glad/gl.h>

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

static void MakeWindow(feWindow& window, unsigned char version, bool useNewStuff, bool visible)
{
	feWindowCreateInfo info;
	info.width = 1280;
	info.height = 720;
	info.title = "3D Engine";
	info.contextMajor = version / 10;
	info.contextMinor = version % 10;
	info.contextForwardCompat = useNewStuff;
	info.contextProfileCore = useNewStuff;
	info.contextDebug = useNewStuff;
	info.visible = visible;

	window = feWindow(info);

	feContext::Load(window);
	feRenderUtil::ClearLoadedFlag();
}

class Game : public feApplication
{
public:
	virtual ~Game() noexcept = default;

	virtual void Init() override
	{
		// Minimum version required is OpenGL 4.0
		unsigned char version = 40;

		{
			feWindow window;
			MakeWindow(window, version, false, false);
			version = feRenderUtil::GetSupportedVersion();
		}

		MakeWindow(m_Window, version, true, true);

		feRenderUtil::LogOpenGLInfo();
		feRenderUtil::InitDefaults();
		feRenderUtil::SetupDebugLogger();

		Sphere sphere(1, 36, 18, false);

		{
			feBufferObjectCreateInfo info;
			info.target = GL_ARRAY_BUFFER;
			info.data = sphere.getVertices();
			info.size = sphere.getVertexSize();

			m_Vbo = info;
		}

		{
			feBufferObjectCreateInfo info;
			info.target = GL_ELEMENT_ARRAY_BUFFER;
			info.data = sphere.getIndices();
			info.size = sphere.getIndexSize();

			m_Ibo = info;
		}
		
		feVertexArrayCreateInfoBufferObjectInfo vaoVboInfo;
		vaoVboInfo.buffer = &m_Vbo;
		vaoVboInfo.stride = 3 * sizeof(float);

		feVertexArrayCreateInfoAttributeInfo vaoAttrInfo;
		vaoAttrInfo.buffer = 0;
		vaoAttrInfo.offset = 0 * sizeof(float);
		vaoAttrInfo.size = 3;
		vaoAttrInfo.type = GL_FLOAT;

		feVertexArrayCreateInfo vaoInfo;
		vaoInfo.attributeInfos = &vaoAttrInfo;
		vaoInfo.attributeInfoCount = 1;
		vaoInfo.count = sphere.getIndexCount();
		vaoInfo.mode = GL_TRIANGLES;
		vaoInfo.vertexBufferInfos = &vaoVboInfo;
		vaoInfo.vertexBufferInfoCount = 1;
		vaoInfo.indexBuffer = &m_Ibo;

		m_Vao = vaoInfo;

		std::string vertSrc = feResourceLoader::LoadTextFile("res/shaders/simple.vert").value();
		std::string fragSrc = feResourceLoader::LoadTextFile("res/shaders/simple.frag").value();

		feShader shaders[2];

		feShaderCreateInfo shaderInfo;
		shaderInfo.type = GL_VERTEX_SHADER;
		std::string_view view = vertSrc;
		shaderInfo.sources = &view;
		shaderInfo.sourceCount = 1;

		shaders[0] = feShader(shaderInfo);

		shaderInfo.type = GL_FRAGMENT_SHADER;
		view = fragSrc;

		shaders[1] = feShader(shaderInfo);

		feProgramCreateInfo programInfo;
		programInfo.shaders = shaders;
		programInfo.shaderCount = 2;

		m_Program = programInfo;
	}
	
	virtual void Update() override
	{
		feWindow::PollEvents();

		if (m_Window.ShouldClose()) Stop();

		feRenderUtil::Viewport(m_Window);
		feRenderUtil::Clear();

		glm::mat4 proj = glm::perspective(glm::radians(80.f), m_Window.GetAspect(), 0.1f, 100.0f);

		m_Program.Bind();
		m_Program.Uniform3f("u_Color", { 0, .1f, .5f });
		m_Program.UniformMat4f("u_Proj", proj);
		m_Vao.Bind();
		m_Vao.Draw();

		m_Window.SwapBuffers();
	}

	virtual double GetTime() override
	{
		return feWindow::GetTime();
	}
private:
	feWindow m_Window;

	feVertexArray m_Vao;
	feBufferObject m_Vbo;
	feBufferObject m_Ibo;
	feProgram m_Program;
};

feApplication* feApplication::CreateInstance()
{
	return new Game;
}

void feApplication::DeleteInstance(feApplication* application)
{
	delete application;
}