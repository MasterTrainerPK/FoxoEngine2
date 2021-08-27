#include "Texture.h"

#include <utility>

#include <glad/gl.h>

#include <glm/glm.hpp>

#include "../math/Math.h"
#include "../Log.h"
#include "Util.h"

feTexture::feTexture(unsigned int handle, unsigned int target)
	: m_Handle(handle), m_Target(target)
{
}

feTexture::feTexture(const feTextureCreateInfo& info)
{
	m_Target = info.target;

	if (feRenderUtil::GetSupportedVersion() >= 45)
	{
		int maxLevel = feMath::FastFloor(glm::log2(static_cast<float>(feMath::Max(info.width, info.height))));
		glCreateTextures(info.target, 1, &m_Handle);
		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTextureStorage2D(m_Handle, maxLevel + 1, info.internalFormat, info.width, info.height);
	}
	else
	{
		glGenTextures(1, &m_Handle);
		glBindTexture(m_Target, m_Handle);
		glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexImage2D(m_Target, 0, info.internalFormat, info.width, info.height, 0, info.format, info.type, info.pixels);
		glBindTexture(m_Target, 0);
	}

	if (info.debugName && feRenderUtil::GetSupportedVersion() >= 43) glObjectLabel(GL_TEXTURE, m_Handle, -1, info.debugName);

	feLog::Trace("Created Texture");
}

feTexture::~feTexture() noexcept
{
	if (m_Handle)
	{
		feLog::Trace("Deleted Texture");
		glDeleteTextures(1, &m_Handle);
	}
}

feTexture::feTexture(feTexture&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
	std::swap(m_Target, other.m_Target);
}

feTexture& feTexture::operator=(feTexture&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
	std::swap(m_Target, other.m_Target);
	return *this;
}

void feTexture::Bind(unsigned int unit) const
{
	if (feRenderUtil::GetSupportedVersion() >= 45)
	{
		glBindTextureUnit(unit, m_Handle);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(m_Target, m_Handle);
	}
}