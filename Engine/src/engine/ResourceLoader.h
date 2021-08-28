#pragma once

#include <optional>
#include <string>
#include <string_view>

class feImage final
{
public:
	feImage() = default;
	feImage(std::string_view filename, int desiredChannels);
	feImage(const unsigned char* buffer, size_t size, int desiredChannels);

	~feImage() noexcept;

	feImage(const feImage&) = delete;
	feImage& operator=(const feImage&) = delete;
	feImage(feImage&& other) noexcept;
	feImage& operator=(feImage&& other) noexcept;

	int GetWidth() const;
	int GetHeight() const;
	int GetChannels() const;
	const unsigned char* GetPixels() const;
private:
	int m_Width = 0, m_Height = 0, m_Channels = 0;
	unsigned char* m_Pixels = nullptr;
};

namespace feResourceLoader
{
	std::optional<std::string> LoadTextFile(std::string_view filename);
}