#include "VertexArray.h"

#include <utility>

#include <glad/gl.h>

#include "../Log.h"

feVertexArray::feVertexArray(unsigned int handle)
	: m_Handle(handle)
{
}

feVertexArray::feVertexArray(const feVertexArrayCreateInfo& info)
{
	glGenVertexArrays(1, &m_Handle);
	glBindVertexArray(m_Handle);

	for (size_t i = 0; i < info.attributeInfoCount; ++i)
	{
		feVertexArrayCreateInfoAttributeInfo* attributeInfo = info.attributeInfos + i;
		feVertexArrayCreateInfoBufferObjectInfo* bufferInfo = info.vertexBufferInfos + attributeInfo->buffer;

		if (attributeInfo->buffer >= info.vertexBufferInfoCount)
		{
			feLog::Warn("Attempting to access out of bound buffer");
			feLog::Break();

			// Do not attempt to read from this location
			continue;
		}

		bufferInfo->buffer->Bind();
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, attributeInfo->size, attributeInfo->type, GL_FALSE, bufferInfo->stride, (const void*) attributeInfo->offset);
	}

	if (info.indexBuffer) info.indexBuffer->Bind();

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_Count = info.count;
	m_Mode = info.mode;
	m_HasIndexBuffer = info.indexBuffer != nullptr;

	feLog::Trace("Created VertexArray");
}

feVertexArray::~feVertexArray() noexcept
{
	if (m_Handle)
	{
		feLog::Trace("Deleted VertexArray");
		glDeleteVertexArrays(1, &m_Handle);
	}
}

feVertexArray::feVertexArray(feVertexArray&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
	std::swap(m_Mode, other.m_Mode);
	std::swap(m_Count, other.m_Count);
	std::swap(m_HasIndexBuffer, other.m_HasIndexBuffer);
}

feVertexArray& feVertexArray::operator=(feVertexArray&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
	std::swap(m_Mode, other.m_Mode);
	std::swap(m_Count, other.m_Count);
	std::swap(m_HasIndexBuffer, other.m_HasIndexBuffer);
	return *this;
}

void feVertexArray::Bind() const
{
	glBindVertexArray(m_Handle);
}

void feVertexArray::Draw() const
{
	if (m_HasIndexBuffer) glDrawElements(m_Mode, m_Count, GL_UNSIGNED_INT, nullptr);
	else glDrawArrays(m_Mode, 0, m_Count);
}