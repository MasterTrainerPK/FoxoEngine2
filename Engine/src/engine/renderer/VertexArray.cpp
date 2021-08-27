#include "VertexArray.h"

#include <utility>

#include <glad/gl.h>

#include "../Log.h"
#include "Util.h"

feVertexArray::feVertexArray(unsigned int handle)
	: m_Handle(handle)
{
}

feVertexArray::feVertexArray(const feVertexArrayCreateInfo& info)
{
	// Use DSA functions if supported
	if (feRenderUtil::GetSupportedVersion() >= 45)
	{
		glCreateVertexArrays(1, &m_Handle);

		for (size_t i = 0; i < info.vertexBufferInfoCount; ++i)
		{
			feVertexArrayCreateInfoBufferObjectInfo* bufferInfo = info.vertexBufferInfos + i;
			glVertexArrayVertexBuffer(m_Handle, i, bufferInfo->buffer->m_Handle, 0, bufferInfo->stride);
		}

		for (size_t i = 0; i < info.attributeInfoCount; ++i)
		{
			feVertexArrayCreateInfoAttributeInfo* attributeInfo = info.attributeInfos + i;

			glEnableVertexArrayAttrib(m_Handle, i);
			glVertexArrayAttribFormat(m_Handle, i, attributeInfo->size, attributeInfo->type, GL_FALSE, attributeInfo->offset);
			glVertexArrayAttribBinding(m_Handle, i, attributeInfo->buffer);
		}

		if (info.indexBuffer) glVertexArrayElementBuffer(m_Handle, info.indexBuffer->m_Handle);
	}
	else
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
			glEnableVertexAttribArray(static_cast<GLuint>(i));
			glVertexAttribPointer(static_cast<GLuint>(i), attributeInfo->size, attributeInfo->type, GL_FALSE, bufferInfo->stride, (const void*) (intptr_t) attributeInfo->offset);
		}

		if (info.indexBuffer) info.indexBuffer->Bind();

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	m_Count = info.count;
	m_Mode = info.mode;
	m_HasIndexBuffer = info.indexBuffer != nullptr;

	if (info.debugName && feRenderUtil::GetSupportedVersion() >= 43) glObjectLabel(GL_VERTEX_ARRAY, m_Handle, -1, info.debugName);

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