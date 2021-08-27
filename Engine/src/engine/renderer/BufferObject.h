#pragma once

struct feBufferObjectCreateInfo final
{
	unsigned int target = 0;
	const void* data = nullptr;
	size_t size = 0;

	const char* debugName = nullptr;
};

class feBufferObject final
{
public:
	feBufferObject(unsigned int handle = 0, unsigned int target = 0);
	feBufferObject(const feBufferObjectCreateInfo& info);
	~feBufferObject() noexcept;

	feBufferObject(const feBufferObject&) = delete;
	feBufferObject& operator=(const feBufferObject&) = delete;

	feBufferObject(feBufferObject&& other) noexcept;
	feBufferObject& operator=(feBufferObject&& other) noexcept;

	void Bind() const;
private:
	unsigned int m_Handle = 0;
	unsigned int m_Target = 0;

	friend class feVertexArray;
};