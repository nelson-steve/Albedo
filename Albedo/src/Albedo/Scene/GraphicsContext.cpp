#include "AlbedoPreCompiledHeader.h"

#include "Albedo/Renderer/GraphicsContext.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Albedo/Renderer/Renderer.h"

namespace Albedo {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    Albedo_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_unique<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		Albedo_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}