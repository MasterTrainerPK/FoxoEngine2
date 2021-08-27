#include "Texture.h"

#include <utility>

#include <glad/gl.h>

#include "../Log.h"
#include "Util.h"

feTexture::feTexture(unsigned int handle, unsigned int target)
	: m_Handle(handle), m_Target(target)
{
}

feTexture::feTexture(const feTextureCreateInfo& info)
{
	m_Target = info.target;

	glGenTextures(1, &m_Handle);
	glBindTexture(m_Target, m_Handle);

	glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexImage2D(m_Target, 0, info.internalFormat, info.width, info.height, 0, info.format, info.type, info.pixels);

	glBindTexture(m_Target, 0);

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
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(m_Target, m_Handle);
}