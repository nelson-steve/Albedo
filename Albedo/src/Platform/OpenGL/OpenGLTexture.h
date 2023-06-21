#pragma once

#include "Albedo/Renderer/Texture.h"

namespace Albedo {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureConfiguration& config);
		OpenGLTexture2D(unsigned int width, unsigned int height);
		OpenGLTexture2D(const std::string& path, bool flipped);
		OpenGLTexture2D(const std::vector<std::string> faces);
		virtual ~OpenGLTexture2D();


		virtual unsigned int getWidth() const override { return m_Width; }
		virtual unsigned int getHeight() const override { return m_Height; }
		virtual const std::string& GetPath() const override { return m_Path; }
		virtual const std::string& GetName() const override { return m_Name; }

		virtual void SetData(void* data, unsigned int size) override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_TextureID == ((OpenGLTexture2D&)other).m_TextureID;
		}

		virtual uint32_t GetTextureID() const override { return m_TextureID; }

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void Unbind() const override;
	private:

		std::string m_Path;
		std::string m_Name;
		bool isCubemap = false;
		uint32_t m_TextureID;
		uint32_t m_Width, m_Height;
		uint32_t m_InternalFormat, m_DataFormat;
	};

}