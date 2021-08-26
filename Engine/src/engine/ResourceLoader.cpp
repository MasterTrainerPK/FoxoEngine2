#include "ResourceLoader.h"

#include <fstream>
#include <utility>

#include "../vendor/stb_image.h"
#include "../engine/Log.h"

feImage::feImage(std::string_view filename, int desiredChannels)
{
	m_Pixels = stbi_load(filename.data(), &m_Width, &m_Height, &m_Channels, desiredChannels);
	if (!m_Pixels) feLog::Error("Failed to load image `{}`", filename);
}

feImage::feImage(const unsigned char* buffer, size_t size, int desiredChannels)
{
	m_Pixels = stbi_load_from_memory(buffer, size, &m_Width, &m_Height, &m_Channels, desiredChannels);
	if (!m_Pixels) feLog::Error("Failed to load image from ptr {}", (const void*) buffer);
}

feImage::~feImage() noexcept
{
	if (m_Pixels) stbi_image_free(m_Pixels);
}

feImage::feImage(feImage&& other) noexcept
{
	std::swap(m_Width, other.m_Width);
	std::swap(m_Height, other.m_Height);
	std::swap(m_Channels, other.m_Channels);
	std::swap(m_Pixels, other.m_Pixels);
}

feImage& feImage::operator=(feImage&& other) noexcept
{
	std::swap(m_Width, other.m_Width);
	std::swap(m_Height, other.m_Height);
	std::swap(m_Channels, other.m_Channels);
	std::swap(m_Pixels, other.m_Pixels);

	return *this;
}

int feImage::GetWidth() const
{
	return m_Width;
}

int feImage::GetHeight() const
{
	return m_Height;
}

int feImage::GetChannels() const
{
	return m_Channels;
}

const unsigned char* feImage::GetPixels() const
{
	return m_Pixels;
}

namespace feResourceLoader
{
	std::optional<std::string> LoadTextFile(std::string_view filename)
	{
		std::FILE* fp;

#if defined(FE_PLAT_WINDOWS)
		errno_t error = fopen_s(&fp, filename.data(), "rb");
		if (error != 0) return {};
#else
		fp = fopen(filename.data(), "rb");
#endif

		if (fp)
		{
			std::string contents;
			std::fseek(fp, 0, SEEK_END);
			contents.resize(std::ftell(fp));
			std::rewind(fp);
			std::fread(&contents[0], 1, contents.size(), fp);
			std::fclose(fp);
			return contents;
		}

		return {};
	}
}