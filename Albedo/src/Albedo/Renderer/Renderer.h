#pragma once

#include "RenderCommand.h"
#include "Albedo/Scene/Components.h"
#include "Albedo/Cameras/EditorCamera.h"
#include <entt.hpp>

namespace Albedo {

	class Renderer 
	{
	public:
		static void   Init(const entt::registry& reg);
		static void	  PreRenderPass(Ref<Shader> depthShader, Ref<ShadowMap> fbo, 
			const entt::registry& reg, const glm::vec3& l, Ref<Texture2D> tex);
		static void   SetupPBR(Camera* camera, const ShaderComponent& shader, const TransformComponent& transform,
			const TextureComponent& texture, const MaterialComponent& material, const std::vector<LightComponent>& lights);
		static void   SetupPlane(Camera* camera, const ShaderComponent& shader, const TransformComponent& transform,
			const TextureComponent& texture, const MaterialComponent& material, const std::vector<LightComponent>& lights,
			const Ref<ShadowMap> shadowMap);
		static void   Setup(Camera* camera, const ShaderComponent& shader, const TransformComponent& transform,
			const TextureComponent& texture, const MaterialComponent& material, const std::vector<LightComponent>& lights);
		static void   Setup(Camera* camera, const ShaderComponent& shader, const TransformComponent& transform,
			const TextureComponent& texture, const MaterialComponent& material);
		static void   SetupSkybox(Camera* camera, const SkyboxComponent skybox, const ShaderComponent shader, const glm::mat4& transform);
		static void   SetupCollider(Camera* camera, const Ref<Shader> shader, const glm::mat4& transform);
		static void	  Setup(Camera* camera, const Ref<Shader> shader, const glm::mat4& transform);
		static void   Render(const Ref<Model> model, const Ref<Shader> shader);
		static void   RenderCollider(const Ref<Model> model, const Ref<Shader> shader);
		static void   Shutdown();
		static void   OnWindowResize(uint32_t width, uint32_t height);
		
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};

}