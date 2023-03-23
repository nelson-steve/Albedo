#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

namespace Albedo {
	
	uint32_t cubeIndices[36] = {
		0, 2, 1, 0, 3, 2, // FRONT
		4, 1, 5, 4, 0, 1, // LEFT
		3, 6, 2, 3, 7, 6, // RIGHT
		5, 2, 1, 5, 6, 2, // TOP
		4, 3, 0, 4, 7, 3, // BOTTOM
		4, 6, 5, 4, 7, 6  // BACK
	};

	struct line {
		float Point1;
		float Point2;
	};

	GLint result;

	const uint32_t SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	uint32_t depthMapFBO;
	uint32_t depthMap;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::InitMaterials(const std::vector<Material*>& materials)
	{
		float skyboxVertices[] =
		{
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

		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		float cubeVertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			 // bottom face
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 // top face
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};

		float planeVertices[] = {
			// positions           // texcoords      // normals         
			 25.0f, -0.5f,  25.0f,  25.0f, 00.0f,  0.0f, 1.0f, 0.0f,
			-25.0f, -0.5f,  25.0f,  00.0f, 00.0f,  0.0f, 1.0f, 0.0f,
			-25.0f, -0.5f, -25.0f,  00.0f, 25.0f,  0.0f, 1.0f, 0.0f,

			 25.0f, -0.5f,  25.0f,  25.0f, 00.0f,  0.0f, 1.0f, 0.0f,
			-25.0f, -0.5f, -25.0f,  00.0f, 25.0f,  0.0f, 1.0f, 0.0f,
			 25.0f, -0.5f, -25.0f,  25.0f, 25.0f,  0.0f, 1.0f, 0.0f,
		};

		float lineVertices[2 * 3] = {
			-0.5f, 0.0f, 0.0f,
			 0.5f, 0.0f, 0.0f
		};
		
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);
		Albedo_Core_INFO("before creating framebuffer: {}", result);

		glGenFramebuffers(1, &depthMapFBO);
		// create depth texture
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		//glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
			Albedo_Core_INFO("Framebuffer complete");
		else
			Albedo_Core_ERROR("WARNING: Framebuffer incomplete");
		//back to default - using 2 as default
		glBindFramebuffer(GL_FRAMEBUFFER, result);

		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);
		Albedo_Core_INFO("after creating framebuffer: {}", result);

		for (Material* material : materials)
		{
			PlaneVertexArray = VertexArray::Create();
			PlaneVertexBuffer = VertexBuffer::Create(planeVertices, sizeof(planeVertices));
			PlaneVertexBuffer->SetLayout
			({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"},
			{ShaderDataType::Float3, "a_Normal"}
			});
			PlaneVertexArray->AddVertexBuffer(PlaneVertexBuffer);

			DebugShader = Shader::Create("Assets/RenderToTextureShader.glsl");
			DebugVertexArray = VertexArray::Create();
			DebugVertexBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
			DebugVertexBuffer->SetLayout
			({
				{ShaderDataType::Float3, "a_Position"},
				{ShaderDataType::Float2, "a_TexCoord"}
			});
			DebugVertexArray->AddVertexBuffer(DebugVertexBuffer);
			DebugShader->Bind();
			DebugShader->SetUniformInt1("depthMap", 0);


			PreRenderShader = Shader::Create("Assets/DepthMapShader.glsl");
			PreRenderTexture = Texture2D::Create("Wood.png");
			PreRenderVertexArray = VertexArray::Create();
			PreRenderVertexBuffer = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
			PreRenderVertexBuffer->SetLayout
			({
				{ShaderDataType::Float3, "a_Position"},
				{ShaderDataType::Float3, "a_Normal"},
				{ShaderDataType::Float2, "a_TexCoord"}
			});
			PreRenderVertexArray->AddVertexBuffer(PreRenderVertexBuffer);


			material->GetMaterialData().Shader_ = Shader::Create(material->GetShaderPath());
			if(material->TextureEnabled() && material->GetMaterialData().TexturePath != "")
				material->GetMaterialData().Texture_ = Texture2D::Create(material->GetTexturePath());
			if (material->TextureEnabled() && material->GetMaterialData().TexturePath2 != "")
				material->GetMaterialData().Texture2_ = Texture2D::Create(material->GetTexturePath2());
			material->GetMaterialData().VertexArray_ = VertexArray::Create();
			material->GetMaterialData().VertexBuffer_ = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
			material->GetMaterialData().VertexBuffer_->SetLayout
			({
				{ShaderDataType::Float3, "a_Position"},
				{ShaderDataType::Float3, "a_Normal"},
				{ShaderDataType::Float2, "a_TexCoord"}
			});
			material->GetMaterialData().VertexArray_->AddVertexBuffer(material->GetMaterialData().VertexBuffer_);
			//material->GetMaterialData().IndexBuffer_ = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices));
			//material->GetMaterialData().VertexArray_->SetIndexBuffer(material->GetMaterialData().IndexBuffer_);
			material->GetMaterialData().Shader_->Bind();
			material->GetMaterialData().Shader_->SetUniformInt1("diffuseTexture", 0);
			material->GetMaterialData().Shader_->SetUniformInt1("shadowMap", 1);
			break;
		}
	}

	void Renderer::Reset()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 2);

		glViewport(0, 0, 1366, 768);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::Setup(const EditorCamera& camera, const Material& material)
	{
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);
		Albedo_Core_INFO("setup: {}", result);

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
		lightProjection = glm::ortho(-orthoValue, orthoValue, -orthoValue, orthoValue, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		material.GetMaterialData().Shader_->Bind();
		material.GetMaterialData().Shader_->SetUniformMat4("u_ProjectionView", camera.GetViewProjection());
		material.GetMaterialData().Shader_->SetUniformFloat3("u_CameraPos", camera.GetPosition());
		material.GetMaterialData().Shader_->SetUniformFloat3("u_LightPos", lightPos);
		material.GetMaterialData().Shader_->SetUniformMat4("u_LightSpaceMatrix", lightSpaceMatrix);
		material.GetMaterialData().Texture_->Bind(0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
	}

	void Renderer::Render(const Material& material)
	{
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);
		Albedo_Core_INFO("render: {}", result);

		material.GetMaterialData().VertexArray_->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
		material.GetMaterialData().VertexArray_->UnBind();
	}

	void Renderer::PreRenderSetup(const EditorCamera& camera, const Material& material)
	{
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);
		Albedo_Core_INFO("Prerender setup: {}", result);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		PreRenderTexture->Bind();

		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);
		Albedo_Core_INFO("Prerender setup: {}", result);

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
		lightProjection = glm::ortho(-orthoValue, orthoValue, -orthoValue, orthoValue, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		PreRenderShader->Bind();
		PreRenderShader->SetUniformMat4("u_LightSpaceMatrix", lightSpaceMatrix);
	}

	void Renderer::PreRenderRender(const Material& material)
	{
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);
		Albedo_Core_INFO("Prerender render: {}", result);

		PreRenderVertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		PreRenderVertexArray->UnBind();
	}

	void Renderer::DebugRender()
	{
		DebugVertexArray->Bind();
		DebugShader->Bind();
		DebugShader->SetUniformFloat("near_plane", near_plane);
		DebugShader->SetUniformFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		
		DebugVertexArray->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	void Renderer::PlaneRender(const Material& material)
	{
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::translate(transform, glm::vec3(0.0f, -11.0f, 0.0));
		transform = glm::rotate(transform, glm::radians(90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		transform = glm::scale(transform, glm::vec3(20.0, 20.0, 20.0));

		material.GetMaterialData().Shader_->Bind();
		material.GetMaterialData().Shader_->SetUniformMat4("u_Transform", transform);
		material.GetMaterialData().VertexArray_->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//PreRenderVertexArray->UnBind();
		//glBindFramebuffer(GL_FRAMEBUFFER, 2);
		//
		//glViewport(0, 0, 1280, 720);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	}

	void Renderer::Shutdown()
	{

	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		Albedo_PROFILE_FUNCTION();
		RenderCommand::SetViewPort(720, 1280, width, height);
	}

	void Renderer::DrawModel()
	{
	}
}