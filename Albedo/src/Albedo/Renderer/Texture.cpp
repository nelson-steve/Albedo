#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/OpenGL/OpenGLTexture2DArray.h"

namespace Albedo {

	Ref<Texture2D> Texture2D::Create(const TextureConfiguration& config)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(config);
		}

		return nullptr;
	}

    Ref<Texture2D> Texture2D::Create(const std::string& path, bool flipped)
    {
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path, flipped);
		}

		return nullptr;
    }

	Ref<Texture2D> Texture2D::Create(const tinygltf::Image& image, const TextureSampler& sampler)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(image, sampler);
		}

		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::vector<std::string> faces)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(faces);
		}

		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(unsigned int width, unsigned int height)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(width, height);
		}

		return nullptr;
	}

	Ref<Texture2DArray> Texture2DArray::Create(unsigned int width, unsigned int height, const std::vector<const void*> data)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2DArray>(width, height, data);
		}

		return nullptr;
	}

}
