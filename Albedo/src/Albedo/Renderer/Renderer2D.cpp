#include "AlbedoPreCompiledHeader.h"

#include "Albedo/Renderer/Renderer2D.h"

#include "Texture.h"

#include "Albedo/Renderer/VertexArray.h"
#include "Albedo/Renderer/Shader.h"
#include "Albedo/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Albedo {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		
		int TexID;

		int EntityID;
	};


	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t TexIndex = 0;
		std::array<Ref<Texture2D>, 32> textureSlots;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		glm::vec4 QuadVertexPositions[4];
	};

	static Renderer2DData s_RendererData;

	void Renderer2D::Init()
	{
		Albedo_PROFILE_FUNCTION();

		s_RendererData.QuadVertexArray = VertexArray::Create();

		s_RendererData.QuadVertexBuffer = VertexBuffer::Create(s_RendererData.MaxVertices * sizeof(QuadVertex));
		s_RendererData.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"	   },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Int,	  "a_TexID"    },
			{ ShaderDataType::Int ,	  "a_EntityID" }
		});
		s_RendererData.QuadVertexArray->AddVertexBuffer(s_RendererData.QuadVertexBuffer);

		s_RendererData.QuadVertexBufferBase = new QuadVertex[s_RendererData.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_RendererData.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_RendererData.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_RendererData.MaxIndices);
		s_RendererData.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		s_RendererData.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_RendererData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_RendererData.WhiteTexture->Bind();

		int32_t samplers[32];

		for (int i = 0; i < 32; i++)
		{
			samplers[i] = i;
		}

		s_RendererData.TextureShader = Shader::Create("Assets/BatchTextureShader.glsl");
		s_RendererData.TextureShader->Bind();
		s_RendererData.TextureShader->SetUniformIntArray("u_Textures", samplers, 32);
		//s_RendererData.TextureShader->SetUniformInt1("u_Texture", 0);

		s_RendererData.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_RendererData.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_RendererData.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_RendererData.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		Albedo_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		Albedo_PROFILE_FUNCTION();

		s_RendererData.TextureShader->Bind();
		s_RendererData.TextureShader->SetUniformMat4("u_ProjectionView", camera.GetProjectionViewMatrix());

		s_RendererData.QuadIndexCount = 0;
		s_RendererData.QuadVertexBufferPtr = s_RendererData.QuadVertexBufferBase;

		s_RendererData.TexIndex = 1;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		Albedo_PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);

		s_RendererData.TextureShader->Bind();
		s_RendererData.TextureShader->SetUniformMat4("u_ProjectionView", viewProj);

		s_RendererData.QuadIndexCount = 0;
		s_RendererData.QuadVertexBufferPtr = s_RendererData.QuadVertexBufferBase;

		s_RendererData.TexIndex = 1;
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		glm::mat4 viewProj = camera.GetViewProjection();

		s_RendererData.TextureShader->Bind();
		s_RendererData.TextureShader->SetUniformMat4("u_ProjectionView", viewProj);

		s_RendererData.QuadIndexCount = 0;
		s_RendererData.QuadVertexBufferPtr = s_RendererData.QuadVertexBufferBase;

		s_RendererData.TexIndex = 1;

		//StartBatch();
	}

	void Renderer2D::EndScene()
	{
		Albedo_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_RendererData.QuadVertexBufferPtr - (uint8_t*)s_RendererData.QuadVertexBufferBase;
		s_RendererData.QuadVertexBuffer->SetData(s_RendererData.QuadVertexBufferBase, dataSize);

		for (int i = 0; i < s_RendererData.TexIndex; i++)
		{
			if (s_RendererData.textureSlots[i])
			{
				s_RendererData.textureSlots[i]->Bind(i);

			}
		}

		Flush();
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		s_RendererData.QuadIndexCount = 0;
		s_RendererData.QuadVertexBufferPtr = s_RendererData.QuadVertexBufferBase;
	}

	void Renderer2D::Flush()
	{
		if (s_RendererData.QuadIndexCount == 0)
			return; // Nothing to draw

		RenderCommand::DrawIndexed(s_RendererData.QuadVertexArray, s_RendererData.QuadIndexCount);
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

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (s_RendererData.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[i];
			s_RendererData.QuadVertexBufferPtr->Color = color;
			s_RendererData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_RendererData.QuadVertexBufferPtr->EntityID = entityID;
			s_RendererData.QuadVertexBufferPtr++;
		}

		s_RendererData.QuadIndexCount += 6;

		/*s_RendererData.TextureShader->SetFloat("u_TilingFactor", 1.0f);
		s_RendererData.WhiteTexture->Bind();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_RendererData.TextureShader->SetMat4("u_Transform", transform);
		s_RendererData.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_RendererData.QuadVertexArray);*/
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		Albedo_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		int textureIndex = 0;

		for (int i = 1; i < s_RendererData.TexIndex; i++)
		{
			if (*s_RendererData.textureSlots[i] == *texture)
			{
				textureIndex = i;
				break;
			}
		}

		if (textureIndex == 0)
		{
			textureIndex = s_RendererData.TexIndex;
			s_RendererData.textureSlots[s_RendererData.TexIndex] = texture;
			s_RendererData.TexIndex++;
		}

		constexpr size_t quadVertexCount = 4;
		//const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		//const float tilingFactor = 1.0f;

		if (s_RendererData.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[i];
			s_RendererData.QuadVertexBufferPtr->Color = tintColor;
			s_RendererData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_RendererData.QuadVertexBufferPtr->TexID = textureIndex;
			s_RendererData.QuadVertexBufferPtr->EntityID = entityID;
			s_RendererData.QuadVertexBufferPtr++;
		}

		s_RendererData.QuadIndexCount += 6;
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

	//void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	//{
	//	DrawQuad(transform, src.Color, entityID);
	//}

	//void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, const Ref<Texture2D> texture, int entityID)
	//{
	//	DrawQuad(transform, texture, entityID);
	//
	//}

}
/*
//----------------------------------------------------------------------------------------------------------------------//

namespace Albedo {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		float TilingFactor;
		glm::vec4 Color;
		int TexIndex;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;
	};

	static Renderer2DData s_RendererData;

	void Renderer2D::Init()
	{
		Albedo_PROFILE_FUNCTION();

		s_RendererData.QuadVertexArray = VertexArray::Create();

		s_RendererData.QuadVertexBuffer = VertexBuffer::Create(s_RendererData.MaxVertices * sizeof(QuadVertex));
		s_RendererData.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Int, "a_TexIndex" }, //float
			{ ShaderDataType::Float, "a_TilingFactor" }
			});
		s_RendererData.QuadVertexArray->AddVertexBuffer(s_RendererData.QuadVertexBuffer);

		s_RendererData.QuadVertexBufferBase = new QuadVertex[s_RendererData.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_RendererData.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_RendererData.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_RendererData.MaxIndices);
		s_RendererData.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		s_RendererData.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_RendererData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_RendererData.MaxTextureSlots];
		for (uint32_t i = 0; i < s_RendererData.MaxTextureSlots; i++)
			samplers[i] = i;
		//BatchTextureShader
		s_RendererData.TextureShader = Shader::Create("Assets/Texture.glsl");
		s_RendererData.TextureShader->Bind();
		s_RendererData.TextureShader->SetUniformIntArray("u_Textures", samplers, s_RendererData.MaxTextureSlots);

		// Set all texture slots to 0
		s_RendererData.TextureSlots[0] = s_RendererData.WhiteTexture;

		s_RendererData.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_RendererData.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_RendererData.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_RendererData.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		Albedo_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		Albedo_PROFILE_FUNCTION();

		s_RendererData.TextureShader->Bind();
		s_RendererData.TextureShader->SetUniformMat4("u_ProjectionView", camera.GetProjectionViewMatrix());

		s_RendererData.QuadIndexCount = 0;
		s_RendererData.QuadVertexBufferPtr = s_RendererData.QuadVertexBufferBase;

		s_RendererData.TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
Albedo_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_RendererData.QuadVertexBufferPtr - (uint8_t*)s_RendererData.QuadVertexBufferBase;
		s_RendererData.QuadVertexBuffer->SetData(s_RendererData.QuadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		s_RendererData.QuadIndexCount = 0;
		s_RendererData.QuadVertexBufferPtr = s_RendererData.QuadVertexBufferBase;

		s_RendererData.TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		// Bind textures
		for (uint32_t i = 0; i < s_RendererData.TextureSlotIndex; i++)
		{
			s_RendererData.TextureSlots[i]->Bind(i);
		}

		RenderCommand::DrawIndexed(s_RendererData.QuadVertexArray, s_RendererData.QuadIndexCount);
		s_RendererData.Stats.DrawCalls++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
Albedo_PROFILE_FUNCTION();

		if (s_RendererData.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[0];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[1];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[2];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[3];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadIndexCount += 6;

		s_RendererData.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
Albedo_PROFILE_FUNCTION();

		if (s_RendererData.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

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

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[0];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[1];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[2];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[3];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadIndexCount += 6;

		s_RendererData.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
Albedo_PROFILE_FUNCTION();

		if (s_RendererData.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[0];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[1];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[2];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[3];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadIndexCount += 6;

		s_RendererData.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		Albedo_PROFILE_FUNCTION();

		if (s_RendererData.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

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

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[0];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[1];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[2];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadVertexBufferPtr->Position = transform * s_RendererData.QuadVertexPositions[3];
		s_RendererData.QuadVertexBufferPtr->Color = color;
		s_RendererData.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_RendererData.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_RendererData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_RendererData.QuadVertexBufferPtr++;

		s_RendererData.QuadIndexCount += 6;

		s_RendererData.Stats.QuadCount++;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_RendererData.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_RendererData.Stats;
	}

}
*/