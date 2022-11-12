#pragma once

#include "RendererAPI.h"
#include <glm/glm.hpp>

namespace Albedo {

	class RenderCommand
	{
	public:
		inline static void Init() { s_RendererAPI->Init(); }
		inline static void ClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }
		inline static void Clear() { s_RendererAPI->Clear(); }

		inline static void SetViewPort(unsigned int x, unsigned int y, unsigned int width, unsigned int  height)
		{ 
			s_RendererAPI->SetViewport(x, y, width, height); 
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray) 
		{ 
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}