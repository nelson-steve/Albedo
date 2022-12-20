#include "AlbedoPreCompiledHeader.h"

#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Albedo {
	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}