#include "AlbedoPreCompiledHeader.h"
#include "ImageLoader.h"

#include <stb_image.h>

namespace Albedo {

	Image::Image()
	{
	}

	Image::Image(const std::string& path)
	{
		data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);

		if (!data)
			Albedo_CORE_ASSERT(false, "");
	}

	void Image::LoadImage(const std::string& path)
	{
		data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);

		if (!data)
			Albedo_CORE_ASSERT(false, "");
	}

}