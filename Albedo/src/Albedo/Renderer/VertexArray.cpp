#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"
#include "VertexArray.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Albedo {
	VertexArray* VertexArray::Create() {
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLVertexArray();
		}
		return nullptr;
	}
}