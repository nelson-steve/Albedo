#pragma once

#include "RenderCommand.h"
#include "Shader.h"
#include "Material.h"
#include "Albedo/Cameras/EditorCamera.h"

namespace Albedo {

	class Renderer 
	{
	public:
		static void Init();
		static void InitMaterials(const std::vector<Material*>& materials);
		static void Setup(const EditorCamera& camera, const Material& material);
		static void Render(const Material& material);
		static void PreRenderSetup(const EditorCamera& camera, const Material& material);
		static void PreRenderRender(const Material& material);
		static void DebugRender();
		static void Reset();
		static void PlaneRender(const Material& material);
		static void Shutdown();

		static void DrawModel();
		static void OnWindowResize(uint32_t width, uint32_t height);
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		inline static Ref<VertexArray> PreRenderVertexArray;
		inline static Ref<VertexBuffer> PreRenderVertexBuffer;
		inline static Ref<Shader> PreRenderShader;
		inline static Ref<Texture2D> PreRenderTexture;

		inline static Ref<VertexArray> DebugVertexArray;
		inline static Ref<VertexBuffer> DebugVertexBuffer;
		inline static Ref<Shader> DebugShader;
		inline static Ref<Texture2D> DebugTexture;

		inline static Ref<VertexArray>  PlaneVertexArray;
		inline static Ref<VertexBuffer> PlaneVertexBuffer;

		inline static glm::vec3 lightPos = glm::vec3(-3.0f, 10.0f, -1.0f);
		inline static float near_plane = 1.0f, far_plane = 50.0f;
		inline static float orthoValue = 10.0f;
	private:
		static const std::vector<Material>& m_Materials;

		inline static glm::vec3 translations[100];
	};

}