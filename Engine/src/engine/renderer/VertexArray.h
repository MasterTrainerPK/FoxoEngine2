#pragma once

#include "BufferObject.h"

struct feVertexArrayCreateInfoBufferObjectInfo final
{
	feBufferObject* buffer = nullptr;
	unsigned int stride = 0;
};

struct feVertexArrayCreateInfoAttributeInfo final
{
	size_t buffer = 0;
	unsigned int size = 0;
	unsigned int type = 0;
	unsigned int offset = 0;
};

struct feVertexArrayCreateInfo final
{
	feVertexArrayCreateInfoBufferObjectInfo* vertexBufferInfos = nullptr;
	size_t vertexBufferInfoCount = 0;
	feVertexArrayCreateInfoAttributeInfo* attributeInfos = nullptr;
	size_t attributeInfoCount = 0;
	feBufferObject* indexBuffer = nullptr;
	unsigned int count = 0;
	unsigned int mode = 0;
};

class feVertexArray final
{
public:
	feVertexArray(unsigned int handle = 0);
	feVertexArray(const feVertexArrayCreateInfo& info);
	~feVertexArray() noexcept;

	feVertexArray(const feVertexArray&) = delete;
	feVertexArray& operator=(const feVertexArray&) = delete;
	feVertexArray(feVertexArray&& other) noexcept;
	feVertexArray& operator=(feVertexArray&& other) noexcept;

	void Bind() const;
	void Draw() const;
private:
	unsigned int m_Handle = 0;
	unsigned int m_Mode = 0;
	unsigned int m_Count = 0;
	bool m_HasIndexBuffer = false;
};