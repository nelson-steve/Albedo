#include "AlbedoPreCompiledHeader.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Albedo {
	  RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	  Scope<RendererAPI> RendererAPI::Create()
	  {
		  switch (s_API)
		  {
		  case RendererAPI::API::None: return nullptr;
		  case RendererAPI::API::OpenGL:  return std::make_unique<OpenGLRendererAPI>();
		  }

		  return nullptr;
	  }
}