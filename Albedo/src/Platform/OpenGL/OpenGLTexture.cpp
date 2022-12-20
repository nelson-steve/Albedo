#include "AlbedoPreCompiledHeader.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>

#include "stb_image.h"


namespace Albedo {

	OpenGLTexture2D::OpenGLTexture2D(unsigned int width, unsigned int height)
		:m_Width(width),m_Height(height)
	{
		Albedo_PROFILE_FUNCTION();
		glGenTextures(1, &m_TextureID);
		this->Bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		:m_Path(path)
	{
		Albedo_PROFILE_FUNCTION();
		//int width, height, channels;
		//stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		//if (!data) Albedo_Core_WARN("Didn't load");

		//m_Width = width;
		//m_Height = height;

		glGenTextures(1, &m_TextureID);
		this->Bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(1);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			//glTexSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			Albedo_Core_WARN("Failed to load texture");
		}
		//stbi_image_free(data);

		//glTexStorage2D(m_TextureID, 1, GL_RGB8, m_Width, m_Height);

		//glTexParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//glTexSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		Albedo_PROFILE_FUNCTION();
		glDeleteTextures(1, &m_TextureID);
	}

	void OpenGLTexture2D::SetData(void* data, unsigned int size)
	{
		Albedo_PROFILE_FUNCTION();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		Albedo_PROFILE_FUNCTION();
		//glBindTextureUnit(slot, m_TextureID);
		//glActiveTexture(GL_TEXTURE0 + slot);
		//glBindTexture(GL_TEXTURE_2D, m_TextureID);
		//glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		//glBindTexture(slot, m_TextureID);
	}

}