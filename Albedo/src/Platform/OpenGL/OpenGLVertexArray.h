#pragma once

#include "Albedo/Renderer/VertexArray.h"

namespace Albedo {

	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual inline const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const
		{ 
			return m_VertexBuffers; 
		}
		virtual inline const Ref<IndexBuffer>& GetIndexBuffer() const
		{
			return m_IndexBuffer;
		}
	private:
		uint32_t m_RendererID;
		uint32_t m_VertexBufferIndex = 0;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}