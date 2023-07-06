#include "AlbedoPreCompiledHeader.h"

#include "Buffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Albedo {

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: //assert
				//case RendererAPI::API::Direct3d: //assert
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(size);
		}
		//assert();
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: //assert
			//case RendererAPI::API::Direct3d: //assert
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		}
		//assert();
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const std::vector<float>& vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: //assert
			//case RendererAPI::API::Direct3d: //assert
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		}
		//assert();
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const std::vector<glm::vec3>& vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: //assert
			//case RendererAPI::API::Direct3d: //assert
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		}
		//assert();
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const std::vector<uint32_t>& instances, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: //assert
			//case RendererAPI::API::Direct3d: //assert
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(instances, size);
		}
		//assert();
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const std::vector<glm::mat4>& transforms, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: //assert
			//case RendererAPI::API::Direct3d: //assert
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(transforms, size);
		}
		//assert();
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const std::vector<glm::vec2>& vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: //assert
			//case RendererAPI::API::Direct3d: //assert
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		}
		//assert();
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const std::vector<int>& ids, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: //assert
			//case RendererAPI::API::Direct3d: //assert
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(ids, size);
		}
		//assert();
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: //assert
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLIndexBuffer>(indices, count);
		}
		//assert();
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(const void* data, uint32_t size, uint32_t program, uint32_t bindingPoint)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: //assert
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLUniformBuffer>(data, size, program, bindingPoint);
		}
		//assert();
		return nullptr;
	}

}