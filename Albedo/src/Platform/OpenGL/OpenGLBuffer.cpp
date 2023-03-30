#include "AlbedoPreCompiledHeader.h"

#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Albedo {

	//////////////////////////////////////////
	///////////////VertexBuffer///////////////
	//////////////////////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		//For batching
		Albedo_PROFILE_FUNCTION();

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const float* vertices, uint32_t size)
	{
		Albedo_PROFILE_FUNCTION();
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const std::vector<float>& vertices, uint32_t size)
	{
		Albedo_PROFILE_FUNCTION();
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, &vertices[0], GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const std::vector<glm::vec3>& vertices, uint32_t size)
	{
		Albedo_PROFILE_FUNCTION();
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, &vertices[0], GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const std::vector<glm::vec2>& vertices, uint32_t size)
	{
		Albedo_PROFILE_FUNCTION();
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, &vertices[0], GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		Albedo_PROFILE_FUNCTION();
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		Albedo_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}
	void OpenGLVertexBuffer::UnBind() const
	{
		Albedo_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	//////////////////////////////////////////
	////////////////IndexBuffer///////////////
	//////////////////////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		:m_Count(count)
	{
		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
		Albedo_PROFILE_FUNCTION();
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}
	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		Albedo_PROFILE_FUNCTION();
		glDeleteBuffers(1, &m_RendererID);
	}
	void OpenGLIndexBuffer::Bind() const
	{
		Albedo_PROFILE_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}
	void OpenGLIndexBuffer::Unbind() const
	{
		Albedo_PROFILE_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}