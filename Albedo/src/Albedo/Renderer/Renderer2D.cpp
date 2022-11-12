#include "AlbedoPreCompiledHeader.h"

#include "Renderer2D.h"

#include "Albedo/Renderer/VertexArray.h"
#include "Albedo/Renderer/Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Albedo {

	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> FlatColorShader;
	};

	static Renderer2DStorage* s_RendererData;

	void Renderer2D::Init()
	{
		s_RendererData = new Renderer2DStorage();
		s_RendererData->QuadVertexArray = (VertexArray::Create());

		float squareVertices[4 * 7] = {
			-0.5f,  0.5f, 0.0f, 1.0f, 0.2f, 0.0f, 1.0f,
			 0.5f,  0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			-0.5f, -0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.1f, 0.8f, 0.2f, 1.0f
		};

		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		BufferLayout layout =
		{
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"}
		};

		vertexBuffer->SetLayout(layout);
		s_RendererData->QuadVertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int cubeIndices[6] = { 0, 1, 2, 2, 1, 3 };

		Ref<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned int)));
		s_RendererData->QuadVertexArray->SetIndexBuffer(indexBuffer);
		s_RendererData->FlatColorShader = Shader::Create("Assets/TextureSquare.glsl");
	}

	void Renderer2D::Shutdown()
	{
		delete s_RendererData;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
		glm::vec3 pos(-0.5f, 0.0f, 0.0f);
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;

		std::dynamic_pointer_cast<OpenGLShader>(s_RendererData->FlatColorShader)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>
			(s_RendererData->FlatColorShader)->UploadUniformMat4("u_ProjectionView", camera.GetProjectionViewMatrix());
		std::dynamic_pointer_cast<OpenGLShader>(s_RendererData->FlatColorShader)->UploadUniformMat4("u_Transform", transform);
	}

	void Renderer2D::EndScene()
	{

	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		std::dynamic_pointer_cast<OpenGLShader>(s_RendererData->FlatColorShader)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(s_RendererData->FlatColorShader)->UploadUniformFloat4("u_Color", color);

		s_RendererData->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_RendererData->QuadVertexArray);
	}

}