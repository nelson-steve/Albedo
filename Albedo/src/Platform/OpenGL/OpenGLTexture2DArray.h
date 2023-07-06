#pragma once

#include "Albedo/Renderer/Texture.h"

namespace Albedo {

	class OpenGLTexture2DArray : public Texture2DArray
	{
	public:
		OpenGLTexture2DArray(unsigned int width, unsigned int height, const std::vector<const void*> data);

		virtual unsigned int getWidth() const override { return m_Width; }
		virtual unsigned int getHeight() const override { return m_Height; }
		virtual const std::string& GetPath() const override { return m_Path; }
		virtual const std::string& GetName() const override { return m_Name; }

		virtual void SetData(void* data, DataType type) override;
		virtual void GetData(void* data, DataType type) override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_TextureID == ((OpenGLTexture2DArray&)other).m_TextureID;
		}

		virtual uint32_t GetTextureID() const override { return m_TextureID; }

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void Unbind() const override;
	private:
		uint32_t m_TextureID = 0;
		std::string m_Path = "";
		std::string m_Name = "";
		uint32_t m_Width = 0, m_Height = 0;
		uint32_t m_TextureCount = 0;
		uint32_t m_MipLevelCount = 1;
	};

}