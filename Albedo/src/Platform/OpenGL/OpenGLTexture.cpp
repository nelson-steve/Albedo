#include "AlbedoPreCompiledHeader.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>

#include "stb_image.h"
#include <gl/GL.h>


namespace Albedo {

	OpenGLTexture2D::OpenGLTexture2D(const TextureConfiguration& config)
	{
		if (!config.Path.empty() || config.m_NullData)
		{
			if (m_Path.find('/') != std::string::npos)
			{
				size_t pos = m_Path.find_last_of('/');
				m_Name = m_Path.substr(pos + 1, m_Path.size());
			}
			else if (m_Path.find('\\') != std::string::npos)
			{
				size_t pos = m_Path.find_last_of('\\');
				m_Name = m_Path.substr(pos + 1, m_Path.size());
			}
			else
			{
				m_Name = "Custom";
			}
		}

		GLenum dataFormat = GL_NONE;
		GLenum texLayout  = GL_NONE;
		GLenum dataType   = GL_NONE;
		GLenum texType	  = GL_NONE;
		GLint inFormat    = GL_NONE;
		GLint minFilter   = GL_NONE;
		GLint magFilter   = GL_NONE;

		if (config.m_TextureType == Config::TextureType::Cubemap)
			isCubemap = true;

		if (config.m_TextureType == Config::TextureType::Texture2D)
			texType = GL_TEXTURE_2D;
		else if (config.m_TextureType == Config::TextureType::Cubemap)
			texType = GL_TEXTURE_CUBE_MAP;
		else
			Albedo_Core_WARN("Invalid texture type");

		if (config.m_TextureLayout == Config::TextureLayout::Repeat)
			texLayout = GL_REPEAT;
		else if (config.m_TextureLayout == Config::TextureLayout::ClampToEdge)
			texLayout = GL_CLAMP_TO_EDGE;
		else
			Albedo_Core_WARN("Invalid layout type");

		if (config.m_InternalFormat == Config::InternalFormat::RGB)
			inFormat = GL_RGB;
		else if (config.m_InternalFormat == Config::InternalFormat::RGBA)
			inFormat = GL_RGBA;
		else if (config.m_InternalFormat == Config::InternalFormat::RGB16F)
			inFormat = GL_RGBA16F;
		else
			Albedo_Core_WARN("Invalid internal format type");

		if (config.m_DataType == Config::DataType::INT)
			dataType = GL_INT;
		else if (config.m_DataType == Config::DataType::UNSIGNED_BYTE)
			dataType = GL_UNSIGNED_BYTE;
		else if (config.m_DataType == Config::DataType::FLOAT)
			dataType = GL_FLOAT;

		if (config.m_DataFormat == Config::DataFormat::RGB)
			dataFormat = GL_RGB;
		else if (config.m_DataFormat == Config::DataFormat::RGBA)	
			dataFormat = GL_RGBA;
		else if (config.m_DataFormat == Config::DataFormat::RG)
			dataFormat = GL_RG;

		if (config.m_MinFilter == Config::MinMagFilters::LINEAR)
			minFilter = GL_LINEAR;
		else if (config.m_MinFilter == Config::MinMagFilters::LINEAR_MIPMAP_LINEAR)
			minFilter = GL_LINEAR_MIPMAP_LINEAR;

		if (config.m_MagFilter == Config::MinMagFilters::LINEAR)
			magFilter = GL_LINEAR;
		else if (config.m_MagFilter == Config::MinMagFilters::LINEAR_MIPMAP_LINEAR)
			magFilter = GL_LINEAR_MIPMAP_LINEAR;

		glGenTextures(1, &m_TextureID);
		glBindTexture(texType, m_TextureID);

		glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, magFilter);

		glTexParameteri(texType, GL_TEXTURE_WRAP_S, texLayout);
		glTexParameteri(texType, GL_TEXTURE_WRAP_T, texLayout);
		if (config.m_TextureType == Config::TextureType::Cubemap)
			glTexParameteri(texType, GL_TEXTURE_WRAP_R, texLayout);

		if (config.m_NullData && config.m_TextureType == Config::TextureType::Cubemap)
		{
			for (unsigned int i = 0; i < 6; ++i)
			{
				if (!config.m_Width || !config.m_Height)
					Albedo_Core_WARN("texture width or height is zero");
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, inFormat, config.m_Width, config.m_Height, 0, dataFormat, dataType, nullptr);
			}
		}
		else if (config.m_NullData && config.m_TextureType == Config::TextureType::Texture2D)
		{
			if (!config.m_Width || !config.m_Height)
				Albedo_Core_WARN("texture width or height is zero");
			glTexImage2D(texType, 0, inFormat, config.m_Width, config.m_Height, 0, dataFormat, dataType, nullptr);
		}
		else if (!config.m_NullData)
		{
			if (config.Path.empty() && config.Faces.empty())
				Albedo_CORE_ASSERT(false, "no paths specified");
			int width, height, nrChannels;
			unsigned char* dataRGB = nullptr;
			float* dataHDR = nullptr;
			stbi_set_flip_vertically_on_load(config.m_Flipped);
			if (config.m_TextureType == Config::TextureType::Texture2D)
			{
				if (config.m_InternalFormat == Config::InternalFormat::RGB ||
					config.m_InternalFormat == Config::InternalFormat::RGBA)
				{
					dataRGB = stbi_load(config.Path.c_str(), &width, &height, &nrChannels, 0);
				}
				else if (config.m_InternalFormat == Config::InternalFormat::RGB16F)
				{
					dataHDR = stbi_loadf(config.Path.c_str(), &width, &height, &nrChannels, 0);
				}
				if (dataRGB || dataHDR)
				{
					void* data;
					if (dataRGB) data = dataRGB;
					else if (dataHDR) data = dataHDR;

					if (nrChannels == 4)
					{
						glTexImage2D(texType, 0, GL_RGB, width, height, 0, GL_RGBA, dataType, data);
					}
					else if (nrChannels == 3)
					{
						glTexImage2D(texType, 0, GL_RGB, width, height, 0, GL_RGB, dataType, data);
					}
					else
						glTexImage2D(texType, 0, GL_RGB, width, height, 0, GL_RED, dataType, data);
					if (config.m_TextureType == Config::TextureType::Texture2D)
						glGenerateMipmap(GL_TEXTURE_2D);
					stbi_image_free(data);
				}
				else
				{
					Albedo_Core_WARN("Failed to load texture: {}", config.Path);
				}
			}
			else if (config.m_TextureType == Config::TextureType::Cubemap)
			{
				Albedo_CORE_ASSERT(!config.Faces.empty(), "Invalid cubemap paths");
				for (unsigned int i = 0; i < 6; i++)
				{
					unsigned char* data = stbi_load(config.Faces[i].c_str(), &width, &height, &nrChannels, 0);
					if (data)
					{
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, dataType, data);
						stbi_image_free(data);
					}
					else
					{
						Albedo_Core_ERROR("Cubemap texture failed to load at path: ", config.Faces[i]);
						//std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
						stbi_image_free(data);
					}
				}
			}
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(unsigned int width, unsigned int height)
		:m_Width(width),m_Height(height)
	{
		Albedo_PROFILE_FUNCTION();
		glGenTextures(1, &m_TextureID);
		this->Bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool flipped)
		:m_Path(path)
	{
		if (!m_Path.empty())
		{
			if (m_Path.find('/') != std::string::npos)
			{
				size_t pos = m_Path.find_last_of('/');
				m_Name = m_Path.substr(pos + 1, m_Path.size());
			}
			else if (m_Path.find('\\') != std::string::npos)
			{
				size_t pos = m_Path.find_last_of('\\');
				m_Name = m_Path.substr(pos + 1, m_Path.size());
			}
			else
			{
				m_Name = m_Path;
			}
		}
		glGenTextures(1, &m_TextureID);
		this->Bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(flipped);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			if(nrChannels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			else if(nrChannels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			//glTexSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			Albedo_Core_WARN("Failed to load texture: {}", path);
		}

		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::vector<std::string> faces)
	{
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

		int width, height, nrComponents;
		stbi_set_flip_vertically_on_load(0);
		for (unsigned int i = 0; i < 6; i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				Albedo_Core_ERROR("Cubemap texture failed to load at path: ", faces[i]);
				//std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		Albedo_PROFILE_FUNCTION();
		glDeleteTextures(1, &m_TextureID);
	}

	void OpenGLTexture2D::SetData(void* data, unsigned int size)
	{
		Albedo_PROFILE_FUNCTION();
	#if 1
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	#else
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		//HZ_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTexSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	#endif
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		if(isCubemap)
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
		else
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}

	void OpenGLTexture2D::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}