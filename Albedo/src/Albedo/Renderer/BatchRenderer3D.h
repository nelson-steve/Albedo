#pragma once

#include "Albedo/Cameras/Camera.h"
#include "Albedo/Cameras/OrthographicCamera.h"
#include "Albedo/Cameras/EditorCamera.h"
#include "Texture.h"						  

#include "Albedo/Scene/Components.h"

namespace Albedo {
	namespace Batch {

		class BatchRenderer3D
		{
		public:
			static void Init();
			static void Shutdown();

			static void RenderSkybox(const EditorCamera& camera);

			static void BeginScene(const EditorCamera& camera);
			static void BeginScene(const Camera& camera, const glm::mat4& transform);
			static void EndScene(const EditorCamera& camera);

			// Primitives
			static void DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);
		};

	}
}