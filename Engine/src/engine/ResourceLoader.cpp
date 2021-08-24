#include "ResourceLoader.h"

#include <fstream>

namespace feResourceLoader
{
	std::optional<std::string> LoadTextFile(std::string_view filename)
	{
		std::FILE* fp;

#if defined(_MSC_VER)
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