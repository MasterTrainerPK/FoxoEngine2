#pragma once

#include <string_view>
#include <unordered_map>
#include <string>
#include <string_view>
#include <functional>

#include <glm/glm.hpp>

struct feShaderCreateInfo final
{
	unsigned int type = 0;
	std::string_view* sources = nullptr;
	size_t sourceCount = 0;

	const char* debugName = nullptr;
};

class feShader final
{
public:
	//handle to the shader
	feShader(unsigned int handle = 0);
	//creates the usable shader for opengl
	feShader(const feShaderCreateInfo& info);
	~feShader() noexcept;

	feShader(const feShader&) = delete;
	feShader& operator=(const feShader&) = delete;

	feShader(feShader&& other) noexcept;
	feShader& operator=(feShader&& other) noexcept;
private:
	friend class feProgram;

	unsigned int m_Handle = 0;
};

struct feProgramCreateInfo final
{
	feShader* shaders = nullptr;
	size_t shaderCount = 0;

	const char* debugName = nullptr;
};

class feProgram final
{
public:
	feProgram(unsigned int handle = 0);
	feProgram(const feProgramCreateInfo& info);
	~feProgram() noexcept;

	feProgram(const feProgram&) = delete;
	feProgram& operator=(const feProgram&) = delete;

	feProgram(feProgram&& other) noexcept;
	feProgram& operator=(feProgram&& other) noexcept;

	void Bind() const;

	//Gets a uniform's location from it's name
	int GetUniformLocation(std::string_view name) const;
	void Uniform1f(std::string_view name, float v0) const;
	void Uniform2f(std::string_view name, const glm::vec2& v0) const;
	void Uniform3f(std::string_view name, const glm::vec3& v0) const;
	void Uniform4f(std::string_view name, const glm::vec4& v0) const;
	void Uniform1i(std::string_view name, int v0) const;
	void Uniform2i(std::string_view name, const glm::ivec2& v0) const;
	void Uniform3i(std::string_view name, const glm::ivec3& v0) const;
	void Uniform4i(std::string_view name, const glm::ivec4& v0) const;
	void UniformMat2f(std::string_view name, const glm::mat2& v0) const;
	void UniformMat3f(std::string_view name, const glm::mat3& v0) const;
	void UniformMat4f(std::string_view name, const glm::mat4& v0) const;
private:
	unsigned int m_Handle = 0;
	std::unordered_map<std::string, int> m_Uniforms;
};