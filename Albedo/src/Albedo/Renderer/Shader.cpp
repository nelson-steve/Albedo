#include "AlbedoPreCompiledHeader.h"

#include "Shader.h"
#include "Albedo/Log.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Albedo/Renderer/Renderer.h"

namespace Albedo {

	Shader* Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			//HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); 
			return nullptr;
		case RendererAPI::API::OpenGL:  return new OpenGLShader(filePath);
		}

		//Albedo_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	Shader* Shader::Create(const char* vertexSrc, const char* fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    
				//HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); 
				return nullptr;
			case RendererAPI::API::OpenGL:  return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		//Albedo_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}