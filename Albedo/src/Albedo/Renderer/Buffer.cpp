#include "AlbedoPreCompiledHeader.h"

#include "Buffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Albedo {

	VertexBuffer* VertexBuffer::Create(float* vertices, unsigned int size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: //assert
			//case RendererAPI::API::Direct3d: //assert
			case RendererAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		}
		//assert();
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(unsigned int* indices, unsigned int size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: //assert
			case RendererAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, size);
		}
		//assert();
		return nullptr;
	}

}