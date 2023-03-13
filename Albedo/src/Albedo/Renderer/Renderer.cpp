#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

namespace Albedo {
	
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

		float cubeVertices[] = 
		{
			-1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f,-1.0f,
			 1.0f,-1.0f,-1.0f,
			 1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f,-1.0f, 1.0f,
			 1.0f,-1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,
			 1.0f,-1.0f,-1.0f,
			 1.0f, 1.0f,-1.0f,
			 1.0f,-1.0f,-1.0f,
			 1.0f, 1.0f, 1.0f,
			 1.0f,-1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f,-1.0f,
			-1.0f, 1.0f,-1.0f,
			 1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			 1.0f,-1.0f, 1.0f
		};

		for (Material* material : materials)
		{
			switch (material->GetMaterialType())
			{
			case MaterialType::Skybox:
			{
				material->GetMaterialData().Shader_ = Shader::Create(material->GetShaderPath());
				material->GetMaterialData().Texture_ = Texture2D::Create(material->GetSkyboxTexturePaths());
				material->GetMaterialData().VertexArray_ = VertexArray::Create();
				material->GetMaterialData().VertexBuffer_ = VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices));
				material->GetMaterialData().VertexBuffer_->SetLayout
				({
					{ShaderDataType::Float3, "a_Position"}
				});
				material->GetMaterialData().VertexArray_->AddVertexBuffer(material->GetMaterialData().VertexBuffer_);
				material->GetMaterialData().Shader_->Bind();
				material->GetMaterialData().Shader_->SetUniformInt1("skybox", 0);
				break;
			}
			case MaterialType::Cube:
			{
				material->GetMaterialData().Shader_ = Shader::Create(material->GetShaderPath());
				material->GetMaterialData().Texture_ = Texture2D::Create(material->GetTexturePath());
				material->GetMaterialData().VertexArray_ = VertexArray::Create();
				material->GetMaterialData().VertexBuffer_ = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
				material->GetMaterialData().VertexBuffer_->SetLayout
				({
					{ShaderDataType::Float3, "a_Position"}
				});
				material->GetMaterialData().VertexArray_->AddVertexBuffer(material->GetMaterialData().VertexBuffer_);
				material->GetMaterialData().Shader_->Bind();
				material->GetMaterialData().Shader_->SetUniformInt1("u_Texture", 0);
				break;
			}
			case MaterialType::Model:
				break;
			case MaterialType::Sprite:
				break;
			case MaterialType::Sphere:
				break;
			case MaterialType::Line:
				break;
			default:
				Albedo_Core_ERROR("Error: Invalid type");
				break;
			}
		}
	}

	void Renderer::Setup(const EditorCamera& camera, const Material& material)
	{
		if (material.GetMaterialType() == MaterialType::Skybox)
		{
			//glDepthMask(GL_FALSE);
			glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
			material.GetMaterialData().Shader_->Bind();
			glm::mat4 projection = camera.GetProjection();
			glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
			material.GetMaterialData().Shader_->SetUniformMat4("view", view);
			material.GetMaterialData().Shader_->SetUniformMat4("projection", projection);

			glBindVertexArray(material.GetMaterialData().VertexArray_->GetRendererID());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, material.GetMaterialData().Texture_->GetTextureID());
		}
		else if (material.GetMaterialType() == MaterialType::Cube)
		{
			material.GetMaterialData().Shader_->Bind();
			material.GetMaterialData().Shader_->SetUniformMat4("u_ProjectionView", camera.GetViewProjection());
			material.GetMaterialData().Shader_->SetUniformFloat4("u_Color", material.GetMaterialData().Color);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), material.GetMaterialData().Position)
				* glm::scale(glm::mat4(1.0f), material.GetMaterialData().Scale);
			material.GetMaterialData().Shader_->SetUniformMat4("u_Transform", transform);
		}

		//s_RendererData.ModelShader = Shader::Create("Assets/ModelShader.glsl");
		//s_RendererData.SuzzaneModel = new Model("Assets/backpack/backpack.obj");
	}

	void Renderer::Render(const Material& material)
	{
		if (material.GetMaterialType() == MaterialType::Skybox)
		{
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDepthMask(GL_TRUE);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS);
		}
		else if (material.GetMaterialType() == MaterialType::Cube)
		{
			//glBindVertexArray(material.GetMaterialData().VertexArray_->GetRendererID());
			//glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

			glBindVertexArray(material.GetMaterialData().VertexArray_->GetRendererID());
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, cubeTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
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
		//s_RendererData.SuzzaneModel->Draw(s_RendererData.ModelShader);
	}
}