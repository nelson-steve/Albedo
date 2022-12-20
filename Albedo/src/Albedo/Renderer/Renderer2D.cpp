#include "AlbedoPreCompiledHeader.h"

#include "Renderer2D.h"

#include "Albedo/Renderer/VertexArray.h"
#include "Albedo/Renderer/Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "RenderCommand.h"
#include "Texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#define BATCH 1

namespace Albedo {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;

		float TexIndex;
		float TilingFactor;
	};

	struct Renderer2DData
	{
		const uint32_t MaxQuads		= 1000;
		const uint32_t MaxVertices  = MaxQuads * 4;
		const uint32_t MaxIndices   = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; //confirmed

		Ref<VertexArray>  QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader>		  FlatColorShader;
		Ref<Shader>		  TextureShader;
		Ref<Texture2D>	  WhiteTexture;

		uint32_t	QuadIndexCount		 = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr  = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;
	};

	static Renderer2DData s_RendererData;

	void Renderer2D::Init()
	{
		////////////////////////-CHEKCING THE AMOUNT OF TEXTURE SLOTS-////////////////////////
		//GLint texture_units;
		//glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
		//Albedo_Core_INFO(texture_units);
		//---------------------------------------------------------------------------------//
		
		//GLint res;
		//glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &res);
		//Albedo_Core_INFO(res);
		//s_RendererData = new Renderer2DData();
		Albedo_PROFILE_FUNCTION();
		s_RendererData.QuadVertexArray = VertexArray::Create();

#ifndef BATCH

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
		vertexBuffer = VertexBuffer::Create(textureVertices, sizeof(textureVertices));
		
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"	   },
			{ ShaderDataType::Float2, "a_TexCoord"	   },
			{ ShaderDataType::Float,  "a_TexIndex"	   },
			{ ShaderDataType::Float,  "a_TilingFactor" }
			});
		s_RendererData.QuadVertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int cubeIndices[6] = { 0, 1, 2, 2, 1, 3 };
		Ref<IndexBuffer> indexBuffer;
		indexBuffer = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(unsigned int));

		s_RendererData.QuadVertexArray->SetIndexBuffer(indexBuffer);
		s_RendererData.TextureShader = Shader::Create("Assets/Texture.glsl");
		s_RendererData.TextureShader->Bind();
		s_RendererData.TextureShader->SetUniformInt1("u_Texture", 0);

#endif

#if BATCH

		s_RendererData.QuadVertexBuffer = VertexBuffer::Create(s_RendererData.MaxVertices * sizeof(QuadVertex));
		s_RendererData.QuadVertexBuffer->SetLayout({

			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"	   },
			{ ShaderDataType::Float2, "a_TexCoord" }

			});

		s_RendererData.QuadVertexArray->AddVertexBuffer(s_RendererData.QuadVertexBuffer);
		s_RendererData.QuadVertexBufferBase = new QuadVertex[s_RendererData.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_RendererData.MaxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_RendererData.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = 0 + offset; //+4
			quadIndices[i + 1] = 1 + offset; //+4
			quadIndices[i + 2] = 2 + offset; //+4
											 //+4
			quadIndices[i + 3] = 2 + offset; //+4
			quadIndices[i + 4] = 3 + offset; //+4
			quadIndices[i + 5] = 0 + offset; //+4

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_RendererData.MaxIndices);
		s_RendererData.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices; //freeing the memory allocated by quadIndices on Heap

		s_RendererData.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_RendererData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		s_RendererData.TextureShader = Shader::Create("Assets/TextureSquare.glsl");
		s_RendererData.TextureShader->Bind();
		s_RendererData.TextureShader->SetUniformInt1("u_Texture", 0);

	#if 0
		s_RendererData.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_RendererData.WhiteTexture->SetData(&whiteTextureData, sizeof(unsigned int));
	#endif

#endif
		//BufferLayout layout =
		//{
		//	{ShaderDataType::Float3, "a_Position"},
		//	{ShaderDataType::Float4, "a_Color"}
		//};

		//vertexBuffer->SetLayout(layout);

		//s_RendererData.QuadVertexBufferBase = new QuadVertex[s_RendererData.MaxVertices];

		//uint32_t* quadIndices = new uint32_t[s_RendererData.MaxIndices];
		
		//uint32_t offset = 0;
		//for(uint32_t i = 0; i < s_RendererData.MaxIndices; i += 6)
		//{
		//	quadIndices[i + 0] = offset + 0;
		//	quadIndices[i + 1] = offset + 1;
		//	quadIndices[i + 2] = offset + 2;
		//
		//	quadIndices[i + 3] = offset + 2;
		//	quadIndices[i + 4] = offset + 3;
		//	quadIndices[i + 5] = offset + 0;
		//
		//	offset += 4;
		//}

		//int32_t samplers[s_RendererData.MaxTextureSlots];
		//for (uint32_t i = 0; i < s_RendererData.MaxTextureSlots; i++)
		//	samplers[i] = i;

		//s_RendererData.TextureShader->SetUniformIntArray("u_Textures", samplers, s_RendererData.MaxTextureSlots);

		//s_RendererData.TextureSlots[0] = s_RendererData.WhiteTexture;
	}

	void Renderer2D::Shutdown()
	{
		Albedo_PROFILE_FUNCTION();
		delete s_RendererData.QuadVertexBufferBase;
		//delete s_RendererData;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		Albedo_PROFILE_FUNCTION();
		s_RendererData.TextureShader->Bind();
		s_RendererData.TextureShader->
			SetUniformMat4("u_ProjectionView", camera.GetProjectionViewMatrix());
#if BATCH
		s_RendererData.QuadIndexCount = 0;
		s_RendererData.QuadVertexBufferPtr = s_RendererData.QuadVertexBufferBase;
		
		//s_RendererData.TextureSlotIndex = 1;
#endif
		//s_RendererData.FlatColorShader->SetUniformMat4("u_Transform", transform);
		//s_RendererData.TextureShader->Bind();
		//s_RendererData.TextureShader->
			//SetUniformMat4("u_ProjectionView", camera.GetProjectionViewMatrix());
	}

	void Renderer2D::EndScene()
	{
#if BATCH
		uint32_t dataSize = (uint8_t*)s_RendererData.QuadVertexBufferPtr - (uint8_t*)s_RendererData.QuadVertexBufferBase;
		s_RendererData.QuadVertexBuffer->SetData(s_RendererData.QuadVertexBufferBase, dataSize);

		Flush();
#endif
		Albedo_PROFILE_FUNCTION();
	}

	void Renderer2D::Flush()
	{
		//for (uint32_t i = 0; i < s_RendererData.TextureSlotIndex; i++)
		//	s_RendererData.TextureSlots[i]->Bind(i);

		RenderCommand::DrawIndexed(s_RendererData.QuadVertexArray, s_RendererData.QuadIndexCount);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		Albedo_PROFILE_FUNCTION();
#ifndef BATCH
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_RendererData.TextureShader->SetUniformFloat4("u_Color", color);
		s_RendererData.TextureShader->SetUniformFloat("u_TilingFactor", 1.0f);
		s_RendererData.WhiteTexture->Bind();

		s_RendererData.TextureShader->SetUniformMat4("u_Transform", transform);

		s_RendererData.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_RendererData.QuadVertexArray);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
#endif

#if BATCH

		const float texIndex = 0.0f;
		const float tilingFactor = 1.0f;
		s_RendererData.QuadVertexBufferPtr->Position = position;
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		//s_RendererData.QuadVertexBufferPtr->TexIndex = texIndex;
		//s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = { position.x + size.x, position.y, 0.0f };
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		//s_RendererData.QuadVertexBufferPtr->TexIndex = texIndex;
		//s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = { position.x + size.x, position.y + size.y, 0.0f };
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		//s_RendererData.QuadVertexBufferPtr->TexIndex = texIndex;
		//s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = { position.x, position.y + size.y, 0.0f };
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		//s_RendererData.QuadVertexBufferPtr->TexIndex = texIndex;
		//s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadIndexCount += 6;

		//s_RendererData.TextureShader->SetUniformMat4("u_Transform", transform);
#endif
		//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
		//glm::vec3 pos(-0.5f, 0.0f, 0.0f);
		//glm::mat4 transform1 = glm::translate(glm::mat4(1.0f), pos) * scale;

	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor , const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		Albedo_PROFILE_FUNCTION();
#if BATCH
		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_RendererData.TextureSlotIndex; i++)
		{
			if (*s_RendererData.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_RendererData.TextureSlotIndex;
			s_RendererData.TextureSlots[s_RendererData.TextureSlotIndex] = texture;
			s_RendererData.TextureSlotIndex++;
		}

		s_RendererData.QuadVertexBufferPtr->Position = position;
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = { position.x + size.x, position.y, 0.0f };
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = { position.x + size.x, position.y + size.y, 0.0f };
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = { position.x, position.y + size.y, 0.0f };
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadIndexCount += 6;
#endif

#if 1
		//s_RendererData.TextureShader->SetUniformFloat4("u_Color", glm::vec4(1.0f));
		s_RendererData.TextureShader->SetUniformFloat4("u_Color", tintColor);
		s_RendererData.TextureShader->SetUniformFloat("u_TilingFactor", tilingFactor);
		texture->Bind();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_RendererData.TextureShader->SetUniformMat4("u_Transform", transform);

		s_RendererData.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_RendererData.QuadVertexArray);
#endif
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		Albedo_PROFILE_FUNCTION();

		s_RendererData.TextureShader->SetUniformFloat4("u_Color", color);
		s_RendererData.TextureShader->SetUniformFloat("u_TilingFactor", 1.0f);
		s_RendererData.WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_RendererData.TextureShader->SetUniformMat4("u_Transform", transform);
		s_RendererData.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_RendererData.QuadVertexArray);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		Albedo_PROFILE_FUNCTION();

		s_RendererData.TextureShader->SetUniformFloat4("u_Color", tintColor);
		s_RendererData.TextureShader->SetUniformFloat("u_TilingFactor", tilingFactor);
		texture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_RendererData.TextureShader->SetUniformMat4("u_Transform", transform);

		s_RendererData.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_RendererData.QuadVertexArray);
	}
}