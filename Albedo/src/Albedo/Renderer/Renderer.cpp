#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
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

		LoadModel("Assets/suzanne/suzanne.obj");
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
		//back to default
		glBindFramebuffer(GL_FRAMEBUFFER, result);

		for (Material* material : materials)
		{
			//Model_ = std::make_shared<Model>("Assets/suzanne/suzzane.obj");


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
		material.GetMaterialData().VertexArray_->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
		material.GetMaterialData().VertexArray_->UnBind();
	}

	void Renderer::PreRenderSetup(const EditorCamera& camera, const Material& material)
	{
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		PreRenderTexture->Bind();

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

	void Renderer::LoadModel(const std::string& file, uint32_t mesh)
	{
		mesh_ = new Mesh;

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uv;
		std::vector<uint32_t> indices;

		std::string inputfile = file;
		tinyobj::ObjReaderConfig reader_config;
		reader_config.mtl_search_path = "./"; // Path to material files

		tinyobj::ObjReader reader;

		if (!reader.ParseFromFile(inputfile, reader_config)) {
			if (!reader.Error().empty()) {
				std::cerr << "TinyObjReader: " << reader.Error();
			}
			exit(1);
		}

		if (!reader.Warning().empty()) {
			std::cout << "TinyObjReader: " << reader.Warning();
		}

		auto& attrib = reader.GetAttrib();
		auto& shapes = reader.GetShapes();
		auto& materials = reader.GetMaterials();

		uint32_t s = 0;
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
				vertices.push_back(glm::vec3(vx, vy, vz));

				// Check if `normal_index` is zero or positive. negative = no normal data
				if (idx.normal_index >= 0) {
					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
					normals.push_back(glm::vec3(nx, ny, nz));
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0) {
					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
					uv.push_back(glm::vec2(tx, ty));
				}

				// Optional: vertex colors
				// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
				// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
				// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
			}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}

		mesh_->SetVertices(vertices);
		//mesh_->SetIndices(indices);
		mesh_->SetUV(uv);
		mesh_->SetNormals(normals);

		mesh_->InitMesh();
#if 0
		

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t>	 shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;

		std::string name = file;
		std::string path = name.substr(0, name.find_last_of("/") + 1);

		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());

		if (!err.empty())
		{
			std::vector<glm::vec3> vertices;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> uv;
			std::vector<uint32_t> indices;

			//for (uint32_t i = 0; i < shapes[mesh].mesh.indices.size(); ++i)
			{
				//indices.push_back(shapes[mesh].mesh.indices[i].normal_index);
				//vertices.push_back(shapes[mesh].mesh.indices[i].vertex_index);
			}

			//for (uint32_t i = 0; i < shapes[mesh].mesh.positions.size(); i += 3)
			{
				//vertices.push_back(glm::vec3(shapes[mesh].mesh.positions[i], shapes[mesh].mesh.positions[i + 1], shapes[mesh].mesh.positions[i + 2]));
			}

			//for (uint32_t i = 0; i < shapes[mesh].mesh.normals.size(); i += 3)
			{
				//normals.push_back(glm::vec3(shapes[mesh].mesh.normals[i], shapes[mesh].mesh.normals[i + 1], shapes[mesh].mesh.normals[i + 2]));
			}

			//for (uint32_t i = 0; i < shapes[mesh].mesh.texcoords.size(); i += 2)
			{
				//uv.push_back(glm::vec2(shapes[mesh].mesh.texcoords[i], 1.0f - shapes[mesh].mesh.texcoords[i + 1]));
			}

			shapes.clear();
			materials.clear();

			//mesh->SetIndices(indices);
			//mesh->SetVertices(vertices);
			//mesh->SetNormals(normals);	
			//mesh->SetUV(uv);
		}
		else
		{
			Albedo_Core_ERROR("Could not load mesh: {}", err.c_str());
		}
#endif

	}

	void Renderer::DrawModel(const EditorCamera& camera)
	{
		ModelShader->Bind();
		glm::mat4 transform(1.0);
		transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.0));
		transform = glm::scale(transform, glm::vec3(1.0f));
		ModelShader->SetUniformMat4  ("u_Transform", transform);
		ModelShader->SetUniformMat4  ("u_ProjectionView", camera.GetViewProjection());
		ModelShader->SetUniformFloat3("u_CameraPosition", camera.GetPosition());

		ModelShader->SetUniformInt1("u_AlbedoMap", 0);
		ModelShader->SetUniformInt1("u_AOMap", 1);
		ModelShader->SetUniformInt1("u_MetallicMap", 2);
		ModelShader->SetUniformInt1("u_NormalMap", 3);
		ModelShader->SetUniformInt1("u_RoughnessMap", 4);

		ModelShader->SetUniformFloat ("u_RoughnessScale", 1.0);
		ModelShader->SetUniformFloat3("u_LightPosition", lightPos);
		ModelShader->SetUniformFloat3("u_LightColor", glm::vec3(1.0, 1.0, 1.0));
		ModelShader->SetUniformFloat ("u_Exposure", 3.5);

		for (int i = 0; i < mesh_->GetTextures().size(); i++)
		{
			mesh_->GetTextures()[i]->Bind(i);
		}

		mesh_->Render(ModelShader);

	}
}