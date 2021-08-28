#include "Shader.h"

#include <utility>
#include <memory>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include "../Log.h"
#include "Util.h"

feShader::feShader(unsigned int handle)
	: m_Handle(handle)
{
}

feShader::feShader(const feShaderCreateInfo& info)
{
	m_Handle = glCreateShader(info.type);

	std::vector<const char*> sources = std::vector<const char*>(info.sourceCount);
	for (size_t i = 0; i < info.sourceCount; ++i) sources[i] = info.sources[i].data();

	glShaderSource(m_Handle, static_cast<GLsizei>(info.sourceCount), sources.data(), nullptr);

	glCompileShader(m_Handle);

	GLint status;
	glGetShaderiv(m_Handle, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE)
	{
		GLint length;
		glGetShaderiv(m_Handle, GL_INFO_LOG_LENGTH, &length);

		std::unique_ptr<GLchar[]> message = std::make_unique<GLchar[]>(length);
		glGetShaderInfoLog(m_Handle, length, &length, message.get());

		feLog::Error("Shader compilation failed: {}", message.get());
	}

	if (info.debugName && feRenderUtil::GetSupportedVersion() >= 43) glObjectLabel(GL_SHADER, m_Handle, -1, info.debugName);

	feLog::Trace("Created Shader");
}

feShader::~feShader() noexcept
{
	if (m_Handle)
	{
		feLog::Trace("Deleted Shader");
		glDeleteShader(m_Handle);
	}
}

feShader::feShader(feShader&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
}

feShader& feShader::operator=(feShader&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
	return *this;
}

feProgram::feProgram(unsigned int handle)
	: m_Handle(handle)
{
}

feProgram::feProgram(const feProgramCreateInfo& info)
{
	m_Handle = glCreateProgram();

	for (size_t i = 0; i < info.shaderCount; ++i)
		glAttachShader(m_Handle, info.shaders[i].m_Handle);

	glLinkProgram(m_Handle);

	for (size_t i = 0; i < info.shaderCount; ++i)
		glDetachShader(m_Handle, info.shaders[i].m_Handle);

	GLint status;
	glGetProgramiv(m_Handle, GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		GLint length;
		glGetProgramiv(m_Handle, GL_INFO_LOG_LENGTH, &length);

		std::unique_ptr<GLchar[]> message = std::make_unique<GLchar[]>(length);
		glGetProgramInfoLog(m_Handle, length, &length, message.get());

		feLog::Error("Program linking failed: {}", message.get());
	}

	if (info.debugName && feRenderUtil::GetSupportedVersion() >= 43) glObjectLabel(GL_PROGRAM, m_Handle, -1, info.debugName);

	feLog::Trace("Created Program");

	// Load uniforms from shaders into cache

	GLint uniformCount = 0;
	glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORMS, &uniformCount);

	if (uniformCount > 0)
	{
		GLint max_name_len = 0;
		GLsizei length = 0;
		GLsizei count = 0;
		GLenum type = GL_NONE;
		glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

		std::unique_ptr<char[]> uniform_name = std::make_unique<char[]>(max_name_len);

		for (GLint i = 0; i < uniformCount; ++i)
		{
			glGetActiveUniform(m_Handle, i, max_name_len, &length, &count, &type, uniform_name.get());

			std::string name = std::string(uniform_name.get());
			GLint location = glGetUniformLocation(m_Handle, name.c_str());
			m_Uniforms.emplace(std::make_pair(std::move(name), location));
		}
	}

	feLog::Trace("Loaded {} uniforms", uniformCount);
	for (const auto& [k, v] : m_Uniforms)
	{
		feLog::Trace(k);
	}
}

feProgram::~feProgram() noexcept
{
	if (m_Handle)
	{
		feLog::Trace("Deleted Program");
		glDeleteProgram(m_Handle);
	}
}

feProgram::feProgram(feProgram&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
	std::swap(m_Uniforms, other.m_Uniforms);
}

feProgram& feProgram::operator=(feProgram&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
	std::swap(m_Uniforms, other.m_Uniforms);
	return *this;
}

void feProgram::Bind() const
{
	glUseProgram(m_Handle);
}

static std::string s_TempUniformContainer(64, '!');

int feProgram::GetUniformLocation(std::string_view name) const
{
	s_TempUniformContainer = name;

	auto result = m_Uniforms.find(s_TempUniformContainer);

	if (result != m_Uniforms.end()) return result->second;

	return -1;
}

void feProgram::Uniform1f(std::string_view name, float v0) const
{
	if (feRenderUtil::GetSupportedVersion() >= 41) glProgramUniform1f(m_Handle, GetUniformLocation(name), v0);
	else glUniform1f(GetUniformLocation(name), v0);
}

void feProgram::Uniform2f(std::string_view name, const glm::vec2& v0) const
{
	if (feRenderUtil::GetSupportedVersion() >= 41) glProgramUniform2fv(m_Handle, GetUniformLocation(name), 1, glm::value_ptr(v0));
	else glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(v0));
}

void feProgram::Uniform3f(std::string_view name, const glm::vec3& v0) const
{
	if (feRenderUtil::GetSupportedVersion() >= 41) glProgramUniform3fv(m_Handle, GetUniformLocation(name), 1, glm::value_ptr(v0));
	else glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(v0));
}

void feProgram::Uniform4f(std::string_view name, const glm::vec4& v0) const
{
	if (feRenderUtil::GetSupportedVersion() >= 41) glProgramUniform4fv(m_Handle, GetUniformLocation(name), 1, glm::value_ptr(v0));
	else glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(v0));
}

void feProgram::Uniform1i(std::string_view name, int v0) const
{
	if (feRenderUtil::GetSupportedVersion() >= 41) glProgramUniform1i(m_Handle, GetUniformLocation(name), v0);
	else glUniform1i(GetUniformLocation(name), v0);
}

void feProgram::Uniform2i(std::string_view name, const glm::ivec2& v0) const
{
	if (feRenderUtil::GetSupportedVersion() >= 41) glProgramUniform2iv(m_Handle, GetUniformLocation(name), 1, glm::value_ptr(v0));
	else glUniform2iv(GetUniformLocation(name), 1, glm::value_ptr(v0));
}

void feProgram::Uniform3i(std::string_view name, const glm::ivec3& v0) const
{
	if (feRenderUtil::GetSupportedVersion() >= 41) glProgramUniform3iv(m_Handle, GetUniformLocation(name), 1, glm::value_ptr(v0));
	else glUniform3iv(GetUniformLocation(name), 1, glm::value_ptr(v0));
}

void feProgram::Uniform4i(std::string_view name, const glm::ivec4& v0) const
{
	if (feRenderUtil::GetSupportedVersion() >= 41) glProgramUniform4iv(m_Handle, GetUniformLocation(name), 1, glm::value_ptr(v0));
	else glUniform4iv(GetUniformLocation(name), 1, glm::value_ptr(v0));
}

void feProgram::UniformMat2f(std::string_view name, const glm::mat2& v0) const
{
	if (feRenderUtil::GetSupportedVersion() >= 41) glProgramUniformMatrix2fv(m_Handle, GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(v0));
	else glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(v0));
}

void feProgram::UniformMat3f(std::string_view name, const glm::mat3& v0) const
{
	if (feRenderUtil::GetSupportedVersion() >= 41) glProgramUniformMatrix3fv(m_Handle, GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(v0));
	else glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(v0));
}

void feProgram::UniformMat4f(std::string_view name, const glm::mat4& v0) const
{
	if (feRenderUtil::GetSupportedVersion() >= 41) glProgramUniformMatrix4fv(m_Handle, GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(v0));
	else glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(v0));
}