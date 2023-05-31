#pragma once

#include "Albedo/Renderer/Buffer.h"

namespace Albedo {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(const float* vertices, uint32_t size);
		OpenGLVertexBuffer(const std::vector<float>& vertices, uint32_t size);
		OpenGLVertexBuffer(const std::vector<glm::vec3>& vertices, uint32_t size);
		OpenGLVertexBuffer(const std::vector<uint32_t>& instances, uint32_t size);
		OpenGLVertexBuffer(const std::vector<glm::vec2>& vertices, uint32_t size);
		OpenGLVertexBuffer(const std::vector<int>& ids, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual inline const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual inline const uint32_t GetRendererID() const override { return m_RendererID; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

		virtual void SetData(const void* data, uint32_t size) override;
	private:
		unsigned int m_RendererID;
		unsigned int m_ElmentsID;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual inline unsigned int GetCount() const { return m_Count; }
	private:
		unsigned int m_RendererID;
		unsigned int m_Count;
	};

}