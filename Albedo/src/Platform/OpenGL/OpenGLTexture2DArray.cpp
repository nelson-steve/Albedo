#include "AlbedoPreCompiledHeader.h"

#include "OpenGLTexture2DArray.h"

#include <glad/glad.h>

namespace Albedo {

	OpenGLTexture2DArray::OpenGLTexture2DArray(unsigned int width, unsigned int height, const std::vector<const void*> data)
	{
		Albedo_CORE_ASSERT(!data.empty(), "");
		m_TextureCount = data.size();

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureID);
		// Allocate the storage.
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, m_MipLevelCount, GL_RGBA8, width, height, m_TextureCount);

		for (int i = 0; i < data.size(); i++)
		{
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, m_MipLevelCount, 0, 0, 0, width, height, i, GL_RGBA, GL_UNSIGNED_BYTE, data[i]);
		}

		//if (m_MipLevelCount > 1)
			glGenerateMipmap(m_TextureID);

		// Always set reasonable texture parameters
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	void OpenGLTexture2DArray::SetData(void* data, DataType type)
	{
		Albedo_CORE_ASSERT(false, "");
		//for (int i = 0; i < data.size(); i++)
		//{
		//	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, m_MipLevelCount, 0, 0, 0, width, height, i, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
		//}
	}

	void OpenGLTexture2DArray::GetData(void* data, DataType type)
	{
	}

	void OpenGLTexture2DArray::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureID);
	}

	void OpenGLTexture2DArray::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

}
