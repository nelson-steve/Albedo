#pragma once

#include <glad/glad.h>
#include "Albedo/Renderer/Texture.h"
#include "Albedo/Renderer/Framebuffer.h"

namespace Albedo
{
	namespace Utils
	{
		template<typename T>
		static uint32_t GetVertexSize(const std::vector<T> data)
		{
			return data.size() * sizeof(T);
		}

		static void PatchParameter(uint32_t noOfPatchPoints)
		{
			glPatchParameteri(GL_PATCH_VERTICES, noOfPatchPoints);
		}

		template<typename T>
		static GLenum AlbedoToOpenGLENUMType(T type)
		{}

		template<>
		static GLenum AlbedoToOpenGLENUMType<Config::TextureType>(Config::TextureType type)
		{
			switch (type)
			{
			case Config::TextureType::Texture2D:
				return GL_TEXTURE_2D;
			case Config::TextureType::Cubemap:
				return GL_TEXTURE_CUBE_MAP;
			default:
				Albedo_Core_ERROR("unknown type");
				break;
			}
		}

		template<>
		static GLenum AlbedoToOpenGLENUMType<Config::DataFormat>(Config::DataFormat type)
		{
			switch (type)
			{
			case Config::DataFormat::RG:
				return GL_RG;
			case Config::DataFormat::RGB:
				return GL_RGB;
			case Config::DataFormat::RGBA:
				return GL_RGBA;
			case Config::DataFormat::RGBA8:
				return GL_RGBA8;
			case Config::DataFormat::DEPTH:
				return GL_DEPTH_COMPONENT;
			default:
				Albedo_Core_ERROR("unknown type");
				break;
			}
		}

		template<>
		static GLenum AlbedoToOpenGLENUMType<Config::TextureLayout>(Config::TextureLayout type)
		{
			switch (type)
			{
			case Config::TextureLayout::Repeat:
				return GL_REPEAT;
			case Config::TextureLayout::ClampToBorder:
				return GL_CLAMP_TO_BORDER;
			case Config::TextureLayout::ClampToEdge:
				return GL_CLAMP_TO_EDGE;
			default:
				Albedo_Core_ERROR("unknown type");
				break;
			}
		}

		template<>
		static GLenum AlbedoToOpenGLENUMType<Config::DataType>(Config::DataType type)
		{
			switch (type)
			{
			case Config::DataType::INT:
				return GL_INT;
			case Config::DataType::FLOAT:
				return GL_FLOAT;
			case Config::DataType::UNSIGNED_BYTE:
				return GL_UNSIGNED_BYTE;
			default:
				Albedo_Core_ERROR("unknown type");
				break;
			}
		}

		template<>
		static GLenum AlbedoToOpenGLENUMType<FramebufferTextureFormat>(FramebufferTextureFormat type)
		{
			switch (type)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				return GL_DEPTH_STENCIL;
			case FramebufferTextureFormat::RED_INTEGER:
				return GL_RED_INTEGER;
			case FramebufferTextureFormat::RENDER_BUFFER:
				return GL_RENDERBUFFER;
			case FramebufferTextureFormat::RGBA8:
				return GL_LINEAR;
			default:
				Albedo_Core_ERROR("unknown type");
				break;
			}
		}

		template<typename T>
		static GLint AlbedoToOpenGLINTType(T type)
		{
		}

		template<>
		static GLint AlbedoToOpenGLINTType(Config::InternalFormat type)
		{
			switch (type)
			{
			case Config::InternalFormat::RGB:
				return GL_RGB;
			case Config::InternalFormat::RGBA:
				return GL_RGBA8;
			case Config::InternalFormat::RGBA8:
				return GL_RGBA8;
			case Config::InternalFormat::RGB16F:
				return GL_RGB16F;
			case Config::InternalFormat::RGBA32F:
				return GL_RGBA32F;
			case Config::InternalFormat::DEPTH:
				return GL_DEPTH_COMPONENT;
			default:
				Albedo_Core_ERROR("unknown type");
				break;
			}
		}

		template<>
		static GLint AlbedoToOpenGLINTType(Config::MinMagFilters type)
		{
			switch (type)
			{
			case Config::MinMagFilters::LINEAR:
				return GL_LINEAR;
			case Config::MinMagFilters::NEAREST:
				return GL_NEAREST;
			case Config::MinMagFilters::LINEAR_MIPMAP_LINEAR:
				return GL_LINEAR_MIPMAP_LINEAR;
			default:
				Albedo_Core_ERROR("unknown type");
				break;
			}
		}

		static GLenum GetGLWrapMode(int32_t wrapMode)
		{
			switch (wrapMode) {
			case -1:
			case 10497:
				return GL_REPEAT;
			case 33071:
				return GL_CLAMP_TO_EDGE;
			case 33648:
				return GL_MIRRORED_REPEAT;
			}

			std::cerr << "Unknown wrap mode for getVkWrapMode: " << wrapMode << std::endl;
			return GL_REPEAT;
		}

		static GLint GetGLFilterMode(int32_t filterMode)
		{
			switch (filterMode) {
			case -1:
			case 9728:
				return GL_NEAREST;
			case 9729:
				return GL_LINEAR;
			case 9984:
				return GL_NEAREST;
			case 9985:
				return GL_NEAREST;
			case 9986:
				return GL_LINEAR;
			case 9987:
				return GL_LINEAR;
			}

			std::cerr << "Unknown filter mode for getVkFilterMode: " << filterMode << std::endl;
			return GL_LINEAR;
		}

	} // Utils namespace
}