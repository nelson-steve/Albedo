#include "AlbedoPreCompiledHeader.h"

#include "Shader.h"
#include "Albedo/Log.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Albedo/Renderer/Renderer.h"

namespace Albedo {

	Ref<Shader> Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			//HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); 
			return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(filePath);
		}

		//Albedo_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	Ref<Shader> Shader::Create(const std::string& name, const char* vertexSrc, const char* fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    
				//HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); 
				return nullptr;
			case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		//Albedo_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		m_Shaders[name] = shader;
	}
	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Albedo::Ref<Albedo::Shader> ShaderLibrary::Load(const std::string& filePath)
	{
		Ref<Shader> shader = Shader::Create(filePath);
		Add(shader);
		return shader;
	}
	Albedo::Ref<Albedo::Shader> ShaderLibrary::Load(const std::string& name, const std::string& filePath)
	{
		Ref<Shader> shader = Shader::Create(filePath);
		Add(name, shader);
		return shader;
	}

	bool ShaderLibrary::Exists(const std::string& name)
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		return m_Shaders[name];
	}
}