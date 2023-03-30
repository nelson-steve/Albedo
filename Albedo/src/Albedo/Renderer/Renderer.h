#pragma once

#include "RenderCommand.h"
#include "Shader.h"
#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Albedo/Cameras/EditorCamera.h"
#include <Albedo/Scene/SceneObject.h>

namespace Albedo {

	class Renderer 
	{
	public:
		static void Init(const std::vector<Ref<SceneObject>> scnObjs);
		static void Setup(const EditorCamera& camera, const Ref<SceneObject> scnObj);
		static void Render(const Ref<SceneObject> scnObj);
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};

}