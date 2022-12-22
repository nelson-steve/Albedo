#pragma once

#include "Albedo/Cameras/OrthographicCamera.h"
#include "Texture.h"

#include <glm/glm.hpp>
namespace Albedo {

	class _Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture);

		struct Stats
		{
			uint32_t DrawCount = 0;
			uint32_t QuadCount = 0;
		};

		static Stats GetStats();
		static void ResetStats();
	};

}