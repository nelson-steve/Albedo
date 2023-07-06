#pragma once

#include <string>
#include <memory>

#include "Albedo/Core/Core.h"


namespace Albedo {

	struct Config
	{
		enum class TextureType
		{
			Texture2D = 0,
			Cubemap
		};

		enum class InternalFormat // GLint
		{
			RGB,
			RGBA,
			RGBA8,
			RGB16F,
			RGBA32F,
			DEPTH
		};

		enum class DataFormat
		{
			RG,
			RGB,
			RGBA,
			RGBA8,
			DEPTH
		};

		enum class TextureLayout
		{
			Repeat,
			ClampToEdge,
			ClampToBorder
		};

		enum class DataType
		{
			UNSIGNED_BYTE,
			FLOAT,
			INT
		};

		enum class MinMagFilters // GLint
		{
			LINEAR,
			NEAREST,
			LINEAR_MIPMAP_LINEAR
		};
	};

	struct TextureConfiguration
	{
		TextureConfiguration() = default;
		TextureConfiguration(Config::TextureType texType, Config::InternalFormat inFormat, Config::TextureLayout texLayout,
			Config::MinMagFilters minFilter, Config::MinMagFilters magFilter, Config::DataType dataType, Config::DataFormat dataFormat, bool nullData, bool flip,
			const std::string& path = "", uint32_t width = 0, uint32_t height = 0)
			:m_TextureType(texType), 
			m_InternalFormat(inFormat), 
			m_TextureLayout(texLayout), 
			m_MinFilter(minFilter), 
			m_MagFilter(magFilter),
			m_DataType(dataType), 
			m_DataFormat(dataFormat), 
			m_NullData(nullData), 
			m_Flipped(flip), 
			Path(path),
			m_Width(width), 
			m_Height(height)
		{
		}
		
		std::string Path = "";
		std::vector<std::string> Faces;
	
		Config::TextureType m_TextureType		= Config::TextureType::Texture2D;
		Config::InternalFormat m_InternalFormat = Config::InternalFormat::RGBA;
		Config::TextureLayout m_TextureLayout	= Config::TextureLayout::ClampToEdge;
		Config::MinMagFilters m_MinFilter		= Config::MinMagFilters::LINEAR;
		Config::MinMagFilters m_MagFilter		= Config::MinMagFilters::LINEAR;
		Config::DataType m_DataType				= Config::DataType::UNSIGNED_BYTE;
		Config::DataFormat m_DataFormat			= Config::DataFormat::RGBA;
		bool m_NullData							= true;
		bool m_Flipped							= false;
		uint32_t m_Width						= 0;
		uint32_t m_Height						= 0;
	};

	class Texture
	{
	public:

		static enum DataType
		{
			BYTE,
			UNSIGNED_BYTE,
			SHORT,
			UNSIGNED_SHORT,
			INT,
			UNSIGNED_INT,
			FLOAT,
		};

		~Texture() = default;

		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;
		virtual const std::string& GetPath() const = 0;
		virtual const std::string& GetName() const = 0;

		virtual void SetData(void* data, DataType type) = 0;
		virtual void GetData(void* data, DataType type) = 0;
		virtual uint32_t GetTextureID() const = 0;

		virtual bool operator==(const Texture& other) const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const TextureConfiguration& config);
		static Ref<Texture2D> Create(const std::string& path, bool flipped);
		static Ref<Texture2D> Create(const std::vector<std::string> faces);
		static Ref<Texture2D> Create(unsigned int width, unsigned int height);
	};

	class Texture2DArray : public Texture
	{
	public:
		static Ref<Texture2DArray> Create(unsigned int width, unsigned int height, const std::vector<const void*> data);
	};

}