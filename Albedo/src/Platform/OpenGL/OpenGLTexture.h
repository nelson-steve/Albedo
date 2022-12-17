#pragma once

#include "Albedo/Renderer/Texture.h"

namespace Albedo {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(unsigned int width, unsigned int height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual unsigned int getWidth() const override { return m_Width; }
		virtual unsigned int getHeight() const override { return m_Height; }

		virtual void SetData(void* data, unsigned int size) override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_TextureID == ((OpenGLTexture2D&)other).m_TextureID;
		}

		virtual void Bind(uint32_t slot = 0) const override;
	private:
		std::string m_Path;
		unsigned int m_Width, m_Height;
		unsigned int m_TextureID;
	};

}