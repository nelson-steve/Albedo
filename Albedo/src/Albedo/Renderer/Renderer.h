#pragma once

#include "RenderCommand.h"
#include "Albedo/Scene/Components.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Platform/OpenGL/ShadowMap.h"
#include "Albedo/Cameras/EditorCamera.h"
#include "RendererConfig.h"
#include <entt.hpp>

namespace Albedo {

	class Renderer 
	{
	public:
		static void   Init(const entt::registry& reg);
		static void	  PreRenderPass(Ref<Shader> depthShader, Ref<ShadowMap> fbo, 
			const entt::registry& reg, const glm::vec3& l, Ref<Texture2D> tex);
		static void   SetupPBR(const EditorCamera& camera, const ShaderComponent& shader, const TransformComponent& transform,
			const TextureComponent& texture, const MaterialComponent& material, const std::vector<LightComponent>& lights);
		static void   SetupPlane(const EditorCamera& camera, const ShaderComponent& shader, const TransformComponent& transform,
			const TextureComponent& texture, const MaterialComponent& material, const std::vector<LightComponent>& lights,
			const Ref<ShadowMap> shadowMap);
		static void   Setup(const EditorCamera& camera, const ShaderComponent& shader, const TransformComponent& transform,
			const TextureComponent& texture, const MaterialComponent& material, const std::vector<LightComponent>& lights);
		static void   Setup(const SceneCamera& camera, const ShaderComponent& shader, const TransformComponent& transform,
			const TextureComponent& texture, const MaterialComponent& material);
		static void	  Setup(const EditorCamera& camera, const Ref<Shader> shader, const glm::mat4& transform);
		static void   RenderOverlay(const Ref<Mesh> mesh);
		static void   Render(const ModelComponent& mesh, const Ref<Shader> shader);
		static GLenum AlbedoDrawTypeToGLType(DrawType type);
		static void   Shutdown();
		static void   OnWindowResize(uint32_t width, uint32_t height);
		
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};

}