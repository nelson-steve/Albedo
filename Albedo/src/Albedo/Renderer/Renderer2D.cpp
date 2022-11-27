#include "AlbedoPreCompiledHeader.h"

#include "Renderer2D.h"

#include "Albedo/Renderer/VertexArray.h"
#include "Albedo/Renderer/Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "RenderCommand.h"
#include "Texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Albedo {

	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> FlatColorShader;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;
	};

	static Renderer2DStorage* s_RendererData;

	void Renderer2D::Init()
	{
		Albedo_PROFILE_FUNCTION();
		s_RendererData = new Renderer2DStorage();
		s_RendererData->QuadVertexArray = (VertexArray::Create());

		float squareVertices[4 * 7] = {
			-0.5f,  0.5f, 0.0f, 1.0f, 0.2f, 0.0f, 1.0f,
			 0.5f,  0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			-0.5f, -0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.1f, 0.8f, 0.2f, 1.0f
		};

		float textureVertices[4 * 5] = {
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f		
		};

		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(textureVertices, sizeof(textureVertices)));

		//BufferLayout layout =
		//{
		//	{ShaderDataType::Float3, "a_Position"},
		//	{ShaderDataType::Float4, "a_Color"}
		//};

		vertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"}
		});

		//vertexBuffer->SetLayout(layout);
		s_RendererData->QuadVertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int cubeIndices[6] = { 0, 1, 2, 2, 1, 3 };

		Ref<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned int)));

		s_RendererData->WhiteTexture = Texture2D::Create(1, 1);
		unsigned int whiteTextureData = 0xffffffff;
		s_RendererData->WhiteTexture->SetData(&whiteTextureData, sizeof(unsigned int));

		s_RendererData->QuadVertexArray->SetIndexBuffer(indexBuffer);
		s_RendererData->TextureShader = Shader::Create("Assets/Texture2.glsl");
		s_RendererData->TextureShader->Bind();
		s_RendererData->TextureShader->SetUniformInt1("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		Albedo_PROFILE_FUNCTION();
		delete s_RendererData;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		Albedo_PROFILE_FUNCTION();
		s_RendererData->TextureShader->Bind();
		s_RendererData->TextureShader->
			SetUniformMat4("u_ProjectionView", camera.GetProjectionViewMatrix());
		//s_RendererData->FlatColorShader->SetUniformMat4("u_Transform", transform);
		//s_RendererData->TextureShader->Bind();
		//s_RendererData->TextureShader->
			//SetUniformMat4("u_ProjectionView", camera.GetProjectionViewMatrix());
	}

	void Renderer2D::EndScene()
	{
		Albedo_PROFILE_FUNCTION();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		Albedo_PROFILE_FUNCTION();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_RendererData->TextureShader->SetUniformFloat4("u_Color", color);
		s_RendererData->TextureShader->SetUniformFloat("u_TilingFactor", 1.0f);
		s_RendererData->WhiteTexture->Bind();
		//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
		//glm::vec3 pos(-0.5f, 0.0f, 0.0f);
		//glm::mat4 transform1 = glm::translate(glm::mat4(1.0f), pos) * scale;

		s_RendererData->TextureShader->SetUniformMat4("u_Transform", transform);

		s_RendererData->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_RendererData->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor , const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		Albedo_PROFILE_FUNCTION();
		s_RendererData->TextureShader->SetUniformFloat4("u_Color", glm::vec4(1.0f));
		s_RendererData->TextureShader->SetUniformFloat4("u_Color", tintColor);
		s_RendererData->TextureShader->SetUniformFloat("u_TilingFactor", tilingFactor);
		texture->Bind();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_RendererData->TextureShader->SetUniformMat4("u_Transform", transform);

		s_RendererData->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_RendererData->QuadVertexArray);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		Albedo_PROFILE_FUNCTION();

		s_RendererData->TextureShader->SetUniformFloat4("u_Color", color);
		s_RendererData->TextureShader->SetUniformFloat("u_TilingFactor", 1.0f);
		s_RendererData->WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_RendererData->TextureShader->SetUniformMat4("u_Transform", transform);
		s_RendererData->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_RendererData->QuadVertexArray);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		Albedo_PROFILE_FUNCTION();

		s_RendererData->TextureShader->SetUniformFloat4("u_Color", tintColor);
		s_RendererData->TextureShader->SetUniformFloat("u_TilingFactor", tilingFactor);
		texture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_RendererData->TextureShader->SetUniformMat4("u_Transform", transform);

		s_RendererData->QuadVertexArray->Bind();
	}
}