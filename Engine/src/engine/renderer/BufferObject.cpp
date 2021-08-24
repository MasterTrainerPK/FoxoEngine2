#include "BufferObject.h"

#include <utility>

#include <glad/gl.h>

#include "../Log.h"

feBufferObject::feBufferObject(unsigned int handle, unsigned int target)
	: m_Handle(handle), m_Target(target)
{
}

feBufferObject::feBufferObject(const feBufferObjectCreateInfo& info)
{
	m_Target = info.target;

	glGenBuffers(1, &m_Handle);
	glBindBuffer(m_Target, m_Handle);
	glBufferData(m_Target, info.size, info.data, GL_STATIC_DRAW);
	glBindBuffer(m_Target, 0);

	feLog::Trace("Created BufferObject");
}

feBufferObject::~feBufferObject() noexcept
{
	if (m_Handle)
	{
		feLog::Trace("Deleted BufferObject");
		glDeleteBuffers(1, &m_Handle);
	}
}

feBufferObject::feBufferObject(feBufferObject&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
	std::swap(m_Target, other.m_Target);
}

feBufferObject& feBufferObject::operator=(feBufferObject&& other) noexcept
{
	std::swap(m_Handle, other.m_Handle);
	std::swap(m_Target, other.m_Target);
	return *this;
}

void feBufferObject::Bind() const
{
	glBindBuffer(m_Target, m_Handle);
}