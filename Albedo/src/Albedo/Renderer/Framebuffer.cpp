#include "AlbedoPreCompiledHeader.h"
#include "Albedo/Renderer/Framebuffer.h"

#include "Albedo/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Albedo {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    Albedo_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLFramebuffer>(spec);
		}

		//Albedo_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}