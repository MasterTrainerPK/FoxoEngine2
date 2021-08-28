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

	int maxLevel = feMath::FastFloor(glm::log2(static_cast<float>(feMath::Max(info.width, info.height))));

	if (feRenderUtil::GetSupportedVersion() >= 45)
	{
		glCreateTextures(info.target, 1, &m_Handle);
		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, info.filterMin);
		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, info.filterMag);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, info.wrap);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, info.wrap);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_R, info.wrap);
		glTextureParameteri(m_Handle, GL_TEXTURE_MAX_LEVEL, maxLevel);

		if (feRenderUtil::IsExtensionSupported("GL_EXT_texture_filter_anisotropic"))
		{
			float val;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &val);
			glTextureParameterf(m_Handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, val);
		}

		glTextureStorage2D(m_Handle, maxLevel + 1, info.internalFormat, info.width, info.height);
		if (info.pixels) glTextureSubImage2D(m_Handle, 0, 0, 0, info.width, info.height, info.format, info.type, info.pixels);
		if (info.mipmaps) glGenerateTextureMipmap(m_Handle);
	}
	else
	{
		glGenTextures(1, &m_Handle);
		glBindTexture(m_Target, m_Handle);
		glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, info.filterMin);
		glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, info.filterMag);
		glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, info.wrap);
		glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, info.wrap);
		glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, info.wrap);
		glTexParameteri(m_Target, GL_TEXTURE_MAX_LEVEL, maxLevel);

		if (feRenderUtil::IsExtensionSupported("GL_EXT_texture_filter_anisotropic"))
		{
			float val;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &val);
			glTextureParameterf(m_Handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, val);
		}

		glTexImage2D(m_Target, 0, info.internalFormat, info.width, info.height, 0, info.format, info.type, info.pixels);
		if (info.mipmaps) glGenerateMipmap(m_Target);
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