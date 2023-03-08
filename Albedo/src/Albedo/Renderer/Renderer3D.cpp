#include "AlbedoPreCompiledHeader.h"

#include "Renderer3D.h"

#include "Texture.h"

#include "Albedo/Renderer/VertexArray.h"
#include "Albedo/Renderer/Shader.h"
#include "Albedo/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

namespace Albedo {

	struct Renderer3DStorage
	{
		Ref<VertexArray> CubeVertexArray;
		Ref<VertexBuffer> CubeVertexBuffer;
		Ref<Shader> FlatColorShader;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;
		Ref<Texture2D> CubeTexture;

		Ref<VertexArray> CubemapVertexArray;
		Ref<VertexBuffer> CubemapVertexBuffer;
		Ref<Shader> CubemapShader;
		Ref<Texture2D> cubemapTexture;

		Ref<VertexArray> SkyboxVertexArray;
		Ref<VertexBuffer> SkyboxVertexBuffer;
		Ref<Shader> SkyboxShader;
	};

	double prevTime = 0.0f, currentTime = 0.0f;

	static Renderer3DStorage s_RendererData;

	void Renderer3D::Init()
	{
		prevTime = glfwGetTime();

#if 1
		float cubemapVertices[] = {
			// positions          // normals
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		};

		//cubemap
		s_RendererData.CubemapVertexArray = VertexArray::Create();
		s_RendererData.CubemapVertexBuffer = VertexBuffer::Create(cubemapVertices, sizeof(cubemapVertices));

		s_RendererData.CubemapVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float3, "a_Normals"}
		});

		s_RendererData.CubemapVertexArray->AddVertexBuffer(s_RendererData.CubemapVertexBuffer);

		s_RendererData.CubemapShader = Shader::Create("Assets/Cubemap.glsl");

		s_RendererData.CubemapShader->Bind();
		s_RendererData.CubemapShader->SetUniformInt1("skybox", 0);

#endif

		//cube
		s_RendererData.CubeVertexArray = VertexArray::Create();

		float cubeVertices[8 * 3] = {
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f
		};

		uint32_t cubeIndices[36] = {
			0, 2, 1, 0, 3, 2, // FRONT
			4, 1, 5, 4, 0, 1, // LEFT
			3, 6, 2, 3, 7, 6, // RIGHT
			5, 2, 1, 5, 6, 2, // TOP
			4, 3, 0, 4, 7, 3, // BOTTOM
			4, 6, 5, 4, 7, 6  // BACK
		};

		s_RendererData.CubeVertexBuffer = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));

		s_RendererData.CubeVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Position"}
		});

		s_RendererData.CubeVertexArray->AddVertexBuffer(s_RendererData.CubeVertexBuffer);

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices));
		s_RendererData.CubeVertexArray->SetIndexBuffer(quadIB);

		s_RendererData.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_RendererData.WhiteTexture->SetData(&whiteTextureData, sizeof(unsigned int));
		s_RendererData.WhiteTexture->Bind();

		s_RendererData.TextureShader = Shader::Create("Assets/CubeShader.glsl");
		s_RendererData.TextureShader->Bind();
		s_RendererData.TextureShader->SetUniformInt1("u_Texture", 0);

		s_RendererData.CubeTexture = Texture2D::Create("TextureSample5.png");


		//skybox setup
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		//skybox
		s_RendererData.SkyboxVertexArray = VertexArray::Create();
		s_RendererData.SkyboxVertexBuffer = VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices));

		s_RendererData.SkyboxVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Position"}
			});

		s_RendererData.SkyboxVertexArray->AddVertexBuffer(s_RendererData.SkyboxVertexBuffer);

		s_RendererData.SkyboxShader = Shader::Create("Assets/Skybox.glsl");

		std::vector<std::string> faces
		{
			"Assets/right.jpg",
			"Assets/left.jpg",
			"Assets/top.jpg",
			"Assets/bottom.jpg",
			"Assets/front.jpg",
			"Assets/back.jpg",
		};
		s_RendererData.cubemapTexture = Texture2D::Create(faces);

		s_RendererData.SkyboxShader->Bind();
		s_RendererData.SkyboxShader->SetUniformInt1("skybox", 0);

	}

	void Renderer3D::RenderSkybox(const EditorCamera& camera)
	{
#if 1
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		s_RendererData.SkyboxShader->Bind();
		glm::mat4 projection = camera.GetProjection();
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		s_RendererData.SkyboxShader->SetUniformMat4("view", view);
		s_RendererData.SkyboxShader->SetUniformMat4("projection", projection);
		// skybox cube
		glBindVertexArray(s_RendererData.SkyboxVertexArray->GetRendererID());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, s_RendererData.cubemapTexture->GetTextureID());
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
#endif
	}

	void Renderer3D::Shutdown()
	{

	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		currentTime = glfwGetTime();

		double passedTime = currentTime - prevTime;

		//Albedo_Core_TRACE("Time: ", passedTime);
		//std::cout << "Time: " << passedTime;

		glm::mat4 viewProj = camera.GetViewProjection();
			
		s_RendererData.TextureShader->Bind();
		s_RendererData.TextureShader->SetUniformMat4("u_ProjectionView", viewProj);

#if 0
		// draw scene as normal
		s_RendererData.CubemapShader->Bind();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection =camera.GetProjection();
		s_RendererData.CubemapShader->SetUniformMat4("model", model);
		s_RendererData.CubemapShader->SetUniformMat4("view", view);
		s_RendererData.CubemapShader->SetUniformMat4("projection", projection);
		// cubes
		glBindVertexArray(s_RendererData.CubemapVertexArray->GetRendererID());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_RendererData.CubeTexture->GetTextureID());
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		s_RendererData.CubemapShader->Bind();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = camera.GetProjection();
		s_RendererData.CubemapShader->SetUniformMat4("model", model);
		s_RendererData.CubemapShader->SetUniformMat4("view", view);
		s_RendererData.CubemapShader->SetUniformMat4("projection", projection);
		s_RendererData.CubemapShader->SetUniformFloat3("cameraPos", camera.GetPosition());
		// cubes
		glBindVertexArray(s_RendererData.CubeVertexArray->GetRendererID());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, s_RendererData.cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

#endif
	}

	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{

	}

	void Renderer3D::EndScene(const EditorCamera& camera)
	{
		RenderSkybox(camera);
	}

	void Renderer3D::DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, size.z });

		s_RendererData.TextureShader->SetUniformFloat4("u_Color", color);
		s_RendererData.WhiteTexture->Bind();

		s_RendererData.TextureShader->SetUniformMat4("u_Transform", transform);

		s_RendererData.CubeVertexArray->Bind();
		RenderCommand::DrawIndexed(s_RendererData.CubeVertexArray);
	}

}