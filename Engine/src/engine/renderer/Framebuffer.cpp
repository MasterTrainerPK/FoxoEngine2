#include "Framebuffer.h"

#include <utility>

#include <glad/gl.h>

#include <glm/glm.hpp>

#include "../math/Math.h"
#include "../Log.h"
#include "Util.h"

feRenderbuffer::feRenderbuffer(unsigned int handle)
	: m_Handle(handle)
{
}

feRenderbuffer::feRenderbuffer(const feRenderbufferCreateInfo& info)
{
	if (feRenderUtil::GetSupportedVersion() >= 45)
	{
		glCreateRenderbuffers(1, &m_Handle);
		glNamedRenderbufferStorageMultisample(m_Handle, info.samples, info.internalFormat, info.width, info.height);
	}
	else
	{
		glGenRenderbuffers(1, &m_Handle);
		glBindRenderbuffer(GL_RENDERBUFFER, m_Handle);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, info.samples, info.internalFormat, info.width, info.height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	if (info.debugName && feRenderUtil::GetSupportedVersion() >= 43) glObjectLabel(GL_RENDERBUFFER, m_Handle, -1, info.debugName);

	feLog::Trace("Created Renderbuffer");
}

feRenderbuffer::~feRenderbuffer() noexcept
{
	if (m_Handle)
	{
		feLog::Trace("Deleted Renderbuffer");
		glDeleteRenderbuffers(1, &m_Handle);
	}
}

feRenderbuffer::feRenderbuffer(feRenderbuffer&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
}

feRenderbuffer& feRenderbuffer::operator=(feRenderbuffer&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
	return *this;
}

feFramebuffer::feFramebuffer(unsigned int handle)
	: m_Handle(handle)
{
}

feFramebuffer::feFramebuffer(const feFramebufferCreateInfo& info)
{
	if (feRenderUtil::GetSupportedVersion() >= 45)
	{
		glCreateFramebuffers(1, &m_Handle);

		for (size_t i = 0; i < info.textureAttachmentCount; ++i)
		{
			feFramebufferCreateInfoTextureAttachmentInfo* textureInfo = info.textureAttachments + i;
			glNamedFramebufferTexture(m_Handle, textureInfo->attachment, textureInfo->texture->m_Handle, textureInfo->level);
		}

		for (size_t i = 0; i < info.renderbufferAttachmentCount; ++i)
		{
			feFramebufferCreateInfoRenderbufferAttachmentInfo* renderbufferInfo = info.renderbufferAttachments + i;
			glNamedFramebufferRenderbuffer(m_Handle, renderbufferInfo->attachment, GL_RENDERBUFFER, renderbufferInfo->renderbuffer->m_Handle);
		}

		if (glCheckNamedFramebufferStatus(m_Handle, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			feLog::Error("Framebuffer incomplete");
		}
	}
	else
	{
		glGenFramebuffers(1, &m_Handle);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);

		for (size_t i = 0; i < info.textureAttachmentCount; ++i)
		{
			feFramebufferCreateInfoTextureAttachmentInfo* textureInfo = info.textureAttachments + i;
			glFramebufferTexture(GL_FRAMEBUFFER, textureInfo->attachment, textureInfo->texture->m_Handle, textureInfo->level);
		}

		for (size_t i = 0; i < info.renderbufferAttachmentCount; ++i)
		{
			feFramebufferCreateInfoRenderbufferAttachmentInfo* renderbufferInfo = info.renderbufferAttachments + i;
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, renderbufferInfo->attachment, GL_RENDERBUFFER, renderbufferInfo->renderbuffer->m_Handle);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			feLog::Error("Framebuffer incomplete");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	if (info.debugName && feRenderUtil::GetSupportedVersion() >= 43) glObjectLabel(GL_FRAMEBUFFER, m_Handle, -1, info.debugName);

	feLog::Trace("Created Framebuffer");
}

feFramebuffer::~feFramebuffer() noexcept
{
	if (m_Handle)
	{
		feLog::Trace("Deleted Framebuffer");
		glDeleteFramebuffers(1, &m_Handle);
	}
}

feFramebuffer::feFramebuffer(feFramebuffer&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
}

feFramebuffer& feFramebuffer::operator=(feFramebuffer&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
	return *this;
}

void feFramebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
}

void feFramebuffer::BindRead() const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Handle);
}

void feFramebuffer::BindDraw() const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Handle);
}

void feFramebuffer::Blit(const feFramebufferBlitInfo& info) const
{
	if (feRenderUtil::GetSupportedVersion() >= 45)
	{
		glBlitNamedFramebuffer(m_Handle, info.dest->m_Handle, info.srcX0, info.srcY0, info.srcX1, info.srcY1, info.dstX0, info.dstY0, info.dstX1, info.dstY1, info.mask, info.filter);
	}
	else
	{
		BindRead();
		info.dest->BindDraw();
		glBlitFramebuffer(info.srcX0, info.srcY0, info.srcX1, info.srcY1, info.dstX0, info.dstY0, info.dstX1, info.dstY1, info.mask, info.filter);
	}
}