#pragma once

#include "RenderCommand.h"
#include "Albedo/Scene/Components.h"
#include "Shader.h"
#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Albedo/Cameras/EditorCamera.h"
#include <entt.hpp>

namespace Albedo {

	class Renderer 
	{
	public:
		static void Init(const entt::registry& reg);
		static void Setup(const EditorCamera& camera, const ShaderComponent& shader, const TransformComponent& transform, const TextureComponent& texture);
		static void Setup(const SceneCamera& camera, const ShaderComponent& shader, const TransformComponent& transform, const TextureComponent& texture);
		static void Render(const MeshComponent& mesh);
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};

}