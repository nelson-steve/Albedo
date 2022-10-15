#include "AlbedoPreCompiledHeader.h"

#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Albedo {
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}