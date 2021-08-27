#pragma once

#include "Texture.h"

struct feRenderbufferCreateInfo final
{
	int width = 0;
	int height = 0;
	unsigned int internalFormat = 0;

	const char* debugName = nullptr;
};

class feRenderbuffer final
{
public:
	feRenderbuffer(unsigned int handle = 0);
	feRenderbuffer(const feRenderbufferCreateInfo& info);
	~feRenderbuffer() noexcept;

	feRenderbuffer(const feRenderbuffer&) = delete;
	feRenderbuffer& operator=(const feRenderbuffer&) = delete;

	feRenderbuffer(feRenderbuffer&& other) noexcept;
	feRenderbuffer& operator=(feRenderbuffer&& other) noexcept;
private:
	unsigned int m_Handle = 0;

	friend class feFramebuffer;
};

struct feFramebufferCreateInfoTextureAttachmentInfo final
{
	feTexture* texture = nullptr;
	unsigned int attachment = 0;
	unsigned int level = 0;
};

struct feFramebufferCreateInfoRenderbufferAttachmentInfo final
{
	feRenderbuffer* renderbuffer = nullptr;
	unsigned int attachment = 0;
};

struct feFramebufferCreateInfo final
{
	feFramebufferCreateInfoTextureAttachmentInfo* textureAttachments = nullptr;
	size_t textureAttachmentCount = 0;
	feFramebufferCreateInfoRenderbufferAttachmentInfo* renderbufferAttachments = nullptr;
	size_t renderbufferAttachmentCount = 0;

	const char* debugName = nullptr;
};

struct feFramebufferBlitInfo
{
	feFramebuffer* dest = nullptr;
	int srcX0 = 0;
	int srcY0 = 0;
	int srcX1 = 0;
	int srcY1 = 0;
	int dstX0 = 0;
	int dstY0 = 0;
	int dstX1 = 0;
	int dstY1 = 0;
	unsigned int mask = 0;
	unsigned int filter = 0;
};

class feFramebuffer final
{
public:
	feFramebuffer(unsigned int handle = 0);
	feFramebuffer(const feFramebufferCreateInfo& info);
	~feFramebuffer() noexcept;

	feFramebuffer(const feFramebuffer&) = delete;
	feFramebuffer& operator=(const feFramebuffer&) = delete;

	feFramebuffer(feFramebuffer&& other) noexcept;
	feFramebuffer& operator=(feFramebuffer&& other) noexcept;

	void Bind() const;
	void BindRead() const;
	void BindDraw() const;

	void Blit(const feFramebufferBlitInfo& info) const;
private:
	unsigned int m_Handle = 0;
};