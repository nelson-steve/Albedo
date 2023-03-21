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
		static void Shutdown();

		static void DrawModel();
		static void OnWindowResize(uint32_t width, uint32_t height);
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		static const std::vector<Material>& m_Materials;

		inline static Ref<VertexArray> _VertexArray;
		inline static Ref<VertexBuffer> _VertexBuffer;
		inline static Ref<Shader> _Shader;
		inline static Ref<Texture2D> _Texture;

		inline static glm::vec3 translations[100];
	};

}