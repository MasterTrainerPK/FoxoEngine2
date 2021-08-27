#pragma once

struct feTextureCreateInfo final
{
	unsigned int target = 0;
	int width = 0;
	int height = 0;
	unsigned int format = 0;
	unsigned int internalFormat = 0;
	unsigned int type = 0;
	const void* pixels = nullptr;

	const char* debugName = nullptr;
};

class feTexture final
{
public:
	feTexture(unsigned int handle = 0, unsigned int target = 0);
	feTexture(const feTextureCreateInfo& info);
	~feTexture() noexcept;

	feTexture(const feTexture&) = delete;
	feTexture& operator=(const feTexture&) = delete;

	feTexture(feTexture&& other) noexcept;
	feTexture& operator=(feTexture&& other) noexcept;

	void Bind(unsigned int unit = 0) const;
private:
	unsigned int m_Handle = 0;
	unsigned int m_Target = 0;
};