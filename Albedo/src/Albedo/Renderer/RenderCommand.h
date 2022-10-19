#pragma once

#include "RendererAPI.h"
#include <glm/glm.hpp>

namespace Albedo {

	class RenderCommand
	{
	public:
		inline static void ClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }
		inline static void Clear() { s_RendererAPI->Clear(); }

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) 
		{ 
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}