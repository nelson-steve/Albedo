#include "AlbedoPreCompiledHeader.h"
/*
#include "_Renderer2D.h"

#include "Albedo/Renderer/VertexArray.h"
#include "Albedo/Renderer/Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "RenderCommand.h"
#include "Texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

namespace Albedo {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float	  TexIndex;

		//float TilingFactor;
	};

	struct Renderer2DData
	{
		Ref<Shader> TextureShader;

		const uint32_t MaxQuads	   = 1000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices  = MaxQuads * 6;


		static const uint32_t MaxTextureSlots = 32; //confirmed

		GLuint QuadVA = 0;
		GLuint QuadIB = 0;
		GLuint QuadVB = 0;

		GLuint   WhiteTexture = 0;
		uint32_t WhiteTextureSlot = 0;

		uint32_t IndexCount = 0;

		QuadVertex* QuadBuffer    = nullptr;
		QuadVertex* QuadBufferPtr = nullptr;

		std::array<uint32_t, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		_Renderer2D::Stats RenderStats;
	};

	static QuadVertex s_QuadVertex;
	static Renderer2DData s_RendererData;

	void _Renderer2D::Init()
	{
		s_RendererData.QuadBuffer = new QuadVertex[s_RendererData.MaxVertices];
		//s_RendererData.TextureSlots = new s_RendererData.TextureSlots[]

		glGenVertexArrays(1, &s_RendererData.QuadVA);
		glBindVertexArray(s_RendererData.QuadVA);

		glGenBuffers(1, &s_RendererData.QuadVB);
		glBindVertexArray(s_RendererData.QuadVA);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertex) * s_RendererData.MaxVertices, nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (const void*)offsetof(QuadVertex, Position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (const void*)offsetof(QuadVertex, Color));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (const void*)offsetof(QuadVertex, TexCoord));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (const void*)offsetof(QuadVertex, TexIndex));

		uint32_t* indices = new uint32_t[s_RendererData.MaxIndices];
		//uint32_t indices[];
		uint32_t offset = 0;
		for (int i = 0; i < s_RendererData.MaxIndices; i += 6)
		{
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;
			
			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;

			offset += 4;
		}

		glGenBuffers(1, &s_RendererData.QuadIB);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_RendererData.QuadIB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glGenTextures(1, &s_RendererData.WhiteTexture);
		//glCreateTextures(GL_TEXTURE_2D, 1, &s_RendererData.WhiteTexture);
		glBindTexture(GL_TEXTURE_2D, s_RendererData.WhiteTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		uint32_t color = 0xffffffff;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

		s_RendererData.TextureSlots[0] = s_RendererData.WhiteTexture;
		for (size_t i = 0; i < s_RendererData.MaxTextureSlots; i++)
			s_RendererData.TextureSlots[i] = 0;

		s_RendererData.TextureShader = Shader::Create("Assets/Texture2.glsl");
		s_RendererData.TextureShader->Bind();

		int samplers[32];
		for (int i = 0; i < 32; i++)
			samplers[i] = i;
		s_RendererData.TextureShader->SetUniformIntArray("u_Texture", samplers, 32);
		//glUniform1iv(loc, 32, samplers);

	}

	void _Renderer2D::Shutdown()
	{
		glDeleteVertexArrays(1, &s_RendererData.QuadVA);
		glDeleteBuffers(1, &s_RendererData.QuadVB);
		glDeleteBuffers(1, &s_RendererData.QuadIB);

		glDeleteTextures(1, &s_RendererData.WhiteTexture);

		delete[] s_RendererData.QuadBuffer;
	}

	void _Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_RendererData.TextureShader->
			SetUniformMat4("u_ProjectionView", camera.GetProjectionViewMatrix());

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0f, 0.0f));
			//* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_RendererData.TextureShader->SetUniformMat4("u_Transform", transform);
		s_RendererData.TextureShader->Bind();

		s_RendererData.QuadBufferPtr = s_RendererData.QuadBuffer;
	}

	void _Renderer2D::EndScene()
	{
		uint32_t dataSize = (uint8_t*)s_RendererData.QuadBufferPtr - (uint8_t*)s_RendererData.QuadBuffer;
		//s_RendererData.QuadBuffer->SetData(s_RendererData.QuadVertexBufferBase, dataSize);
		glBindBuffer(GL_ARRAY_BUFFER, s_RendererData.QuadVB);
		glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, s_RendererData.QuadBuffer);

		Flush();
	}

	void _Renderer2D::Flush()
	{
		for (uint32_t i = 0; i < s_RendererData.MaxTextureSlots; i++)
		{
			auto tex = s_RendererData.TextureSlots[i];
			//glBindTextureUnit(i, s_RendererData.TextureSlots[i]);
			glBindTexture(i, s_RendererData.TextureSlots[i]);
		}

		glBindVertexArray(s_RendererData.QuadVA);
		glDrawElements(GL_TRIANGLES, s_RendererData.IndexCount, GL_UNSIGNED_INT, nullptr);
		s_RendererData.RenderStats.DrawCount++;

		s_RendererData.IndexCount = 0;

		for (uint32_t i = 0; i < s_RendererData.MaxTextureSlots; i++)
		{
			glDrawElements(GL_TRIANGLES, s_RendererData.IndexCount, GL_UNSIGNED_INT, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);

			//RenderCommand::DrawIndexed(s_RendererData.QuadVA, s_RendererData.IndexCount);
		}
	}

	void _Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		if (s_RendererData.IndexCount >= s_RendererData.MaxIndices)
		{
			EndScene();
		}
		const float texIndex = 0.0f;
		s_RendererData.QuadBufferPtr->Position = { position.x, position.y, 0.0f };
		s_RendererData.QuadBufferPtr->Color = color;
		s_RendererData.QuadBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_RendererData.QuadBufferPtr->TexIndex = texIndex;
		s_RendererData.QuadBufferPtr++;

		s_RendererData.QuadBufferPtr->Position = { position.x + size.x, position.y, 0.0f };
		s_RendererData.QuadBufferPtr->Color = color;
		s_RendererData.QuadBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_RendererData.QuadBufferPtr->TexIndex = texIndex;
		s_RendererData.QuadBufferPtr++;

		s_RendererData.QuadBufferPtr->Position = { position.x + size.x, position.y + size.y, 0.0f };
		s_RendererData.QuadBufferPtr->Color = color;
		s_RendererData.QuadBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_RendererData.QuadBufferPtr->TexIndex = texIndex;
		s_RendererData.QuadBufferPtr++;

		s_RendererData.QuadBufferPtr->Position = { position.x, position.y + size.y, 0.0f };
		s_RendererData.QuadBufferPtr->Color = color;
		s_RendererData.QuadBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_RendererData.QuadBufferPtr->TexIndex = texIndex;
		s_RendererData.QuadBufferPtr++;

		s_RendererData.IndexCount += 6;
		s_RendererData.RenderStats.QuadCount++;
	}

	void _Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		if (s_RendererData.IndexCount >= s_RendererData.MaxIndices || s_RendererData.TextureSlotIndex > 31)
		{
			EndScene();
		}

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_RendererData.TextureSlotIndex; i++)
		{
			if (s_RendererData.TextureSlots[i] == texture->GetTextureID())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_RendererData.TextureSlotIndex;
			s_RendererData.TextureSlots[s_RendererData.TextureSlotIndex] = texture->GetTextureID();
			s_RendererData.TextureSlotIndex++;
		}

		const float texIndex = textureIndex;

		s_RendererData.QuadBufferPtr->Position = { position.x, position.y, 0.0f };
		s_RendererData.QuadBufferPtr->Color = color;
		s_RendererData.QuadBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_RendererData.QuadBufferPtr->TexIndex = texIndex;
		s_RendererData.QuadBufferPtr++;

		s_RendererData.QuadBufferPtr->Position = { position.x + size.x, position.y, 0.0f };
		s_RendererData.QuadBufferPtr->Color = color;
		s_RendererData.QuadBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_RendererData.QuadBufferPtr->TexIndex = texIndex;
		s_RendererData.QuadBufferPtr++;

		s_RendererData.QuadBufferPtr->Position = { position.x + size.x, position.y + size.y, 0.0f };
		s_RendererData.QuadBufferPtr->Color = color;
		s_RendererData.QuadBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_RendererData.QuadBufferPtr->TexIndex = texIndex;
		s_RendererData.QuadBufferPtr++;

		s_RendererData.QuadBufferPtr->Position = { position.x, position.y + size.y, 0.0f };
		s_RendererData.QuadBufferPtr->Color = color;
		s_RendererData.QuadBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_RendererData.QuadBufferPtr->TexIndex = texIndex;
		s_RendererData.QuadBufferPtr++;

		s_RendererData.IndexCount += 6;
		s_RendererData.RenderStats.QuadCount++;
	}

}*/