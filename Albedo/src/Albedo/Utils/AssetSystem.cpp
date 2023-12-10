#include "AlbedoPreCompiledHeader.h"

#include "AssetSystem.h"
#include "Albedo/Renderer/Buffer.h"
#include "Albedo/Renderer/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define ASSIMP_BUILD_DEBUG

namespace Albedo {

	AssetSystem::AssetSystem()
	{
		//LoadDefaultQuad();
		//LoadDefaultCube();
		//LoadDefaultCircle();
		//LoadDefaultSkybox();
	}

	AssetSystem::~AssetSystem()
	{

	}

	const Ref<Model> AssetSystem::LoadGLTFModel(const std::string& path) {
		// load model data
		Ref<Model> model = std::make_shared<Model>();
		model->Load(path);

		return model;
	}
#if 0
	const Ref<Mesh> _LoadModelusingAssimp(const std::string& path)
	{
		//auto& m = LoadModel(path);
		//return m;

		if (path == "")
		{
			Albedo_Core_ERROR("Error: Empty path - Mesh");
			return nullptr;
		}

		for (auto& m : m_Meshes)
		{
			if (m && m->GetPath() == path && m->GetPath() != "")
			{
				return m;
			}
		}

		Assimp::Importer importer;
	//	| aiProcess_FlipUVs
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			Albedo_CORE_ASSERT(false, "ASSIMP: Error loading model");
		}

		Ref<Mesh> tempMesh = std::make_shared<Mesh>();
		tempMesh->SetPath(path);

		tempMesh->SetDataSingularityStatus(false);

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<uint32_t> indices;

		for (uint32_t i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh& mesh = *scene->mMeshes[i];
			for (uint32_t i = 0; i < mesh.mNumVertices; i++)
			{
				glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
				// positions
				
				vector.x = mesh.mVertices[i].x;
				vector.y = mesh.mVertices[i].y;
				vector.z = mesh.mVertices[i].z;
				vertices.push_back(vector);
				// normals
				if (mesh.HasNormals())
				{
					vector.x = mesh.mNormals[i].x;
					vector.y = mesh.mNormals[i].y;
					vector.z = mesh.mNormals[i].z;
					normals.push_back(vector);
				}
				// texture coordinates
				if (mesh.mTextureCoords[0]) // does the mesh contain texture coordinates?
				{
					if (mesh.mTextureCoords[1] ||
						mesh.mTextureCoords[2] ||
						mesh.mTextureCoords[3] ||
						mesh.mTextureCoords[4] ||
						mesh.mTextureCoords[5] ||
						mesh.mTextureCoords[6] ||
						mesh.mTextureCoords[7]
						)
					{
						Albedo_CORE_ASSERT(false, "tex");
					}
					glm::vec2 vec;
					vec.x = mesh.mTextureCoords[0][i].x;
					vec.y = mesh.mTextureCoords[0][i].y;
					uvs.push_back(vec);

					// tangent
					//vector.x = mesh->mTangents[i].x;
					//vector.y = mesh->mTangents[i].y;
					//vector.z = mesh->mTangents[i].z;
					//vertex.Tangent = vector;
					//// bitangent
					//vector.x = mesh->mBitangents[i].x;
					//vector.y = mesh->mBitangents[i].y;
					//vector.z = mesh->mBitangents[i].z;
					//vertex.Bitangent = vector;
				}
				else
				{
					Albedo_Core_WARN("no texture coordinates in this model");
					uvs.push_back(glm::vec2(0.0f, 0.0f));
				}
			}
			for (unsigned int i = 0; i < mesh.mNumFaces; i++)
			{
				aiFace face = mesh.mFaces[i];
				// retrieve all indices of the face and store them in the indices vector
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
		}

		tempMesh->SetVertices(vertices);
		tempMesh->SetNormals(normals);
		tempMesh->SetUV(uvs);
		tempMesh->SetIndices(indices);
		
		m_Meshes.push_back(tempMesh);

		return tempMesh;
	}

	const Ref<Mesh> AssetSystem::LoadModel(const std::string& path)
	{
 		if (path == "")
		{
			Albedo_Core_ERROR("Error: Empty path - Mesh");
			return nullptr;
		}

		for (auto& m : m_Meshes)
		{
			if (m && m->GetPath() == path && m->GetPath() != "")
			{
				return m;
			}
		}

		Ref<Mesh> tempMesh = std::make_shared<Mesh>();
		tempMesh->SetPath(path);

		tempMesh->SetDataSingularityStatus(false);

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uv;
		std::vector<uint32_t> indices;

		std::string inputfile = path;
		tinyobj::ObjReaderConfig reader_config;
		size_t pos = path.find_last_of('/');
		std::string mtl_path = path.substr(0, pos + 1);
		reader_config.mtl_search_path = mtl_path; // Path to material files

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

		for (uint32_t s = 0; s < shapes.size(); s++)
		{
			size_t index_offset = 0;
			//if (shapes[s].mesh.num_face_vertices.size() == 0)
			//	continue;
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
		}
		if(!materials.empty())
			tempMesh->SetMaterials(materials);
		tempMesh->SetVertices(vertices);
		tempMesh->SetUV(uv);
		tempMesh->SetNormals(normals);

		m_Meshes.push_back(tempMesh);

		return tempMesh;
	}
#endif
	const Ref<Texture2D> AssetSystem::LoadTexture(const std::string& path, bool flipped)
	{
		if (path == "")
		{
			Albedo_Core_ERROR("Error: Empty path - Texture");
			return nullptr;
		}
		for (auto& tex : m_Textures)
		{
			if (tex->GetPath() == path && tex->GetPath() != "" && tex != nullptr) return tex;
		}

		Ref<Texture2D> tempTexture = Texture2D::Create(path, flipped);
		m_Textures.push_back(tempTexture);
		return tempTexture;
	}

	bool AssetSystem::ValidateShaderPath(const std::string& path)
	{
		if (path.empty())
			return false;
		if (path.find('.') == std::string::npos)
			return false;

		size_t pos = path.find_last_of('.');
		std::string& extension = path.substr(pos + 1, path.size());

		if (extension == "glsl")
			return true;

		return false;
	}

	bool AssetSystem::ValidateTexturePath(const std::string& path)
	{
		if (path.empty())
			return false;
		if (path.find('.') == std::string::npos)
			return false;

		size_t pos = path.find_last_of('.');
		std::string& extension = path.substr(pos + 1, path.size());

		if (extension == "jpg" || extension == "png" || extension == "jpeg")
			return true;

		return false;
	}

	bool AssetSystem::ValidateModelPath(const std::string& path)
	{
		if (path.empty())
			return false;
		if (path.find('.') == std::string::npos)
			return false;

		size_t pos = path.find_last_of('.');
		std::string& extension = path.substr(pos + 1, path.size());

		// currently only support obj
		if (extension == "gltf")
			return true;

		return false;
	}

	const Ref<Shader> AssetSystem::LoadShader(const std::string& path)
	{
		Ref<Shader> tempShader = Shader::Create(path);
		tempShader->SetPath(path);
		m_Shaders.push_back(tempShader);

		return tempShader;
	}
#if 0
	const Ref<Mesh> LoadDefaultCube()
	{
		//if (m_DefaultCubeLoaded) return m_Meshes[0];
		//else m_DefaultCubeLoaded = true;
		//
		//{
		//	m_Meshes.resize(1);
		//	m_Meshes[0] = std::make_shared<Mesh>();
		//	m_Meshes[0]->SetDataSingularityStatus(true);
		//	m_Meshes[0]->SetName("Default Cube");
		//	m_Meshes[0]->SetSingularMeshData(cubeVertices);
		//	m_Meshes[0]->SetVerticesDataLayout
		//	({
		//		{ShaderDataType::Float3, "a_Position"},
		//		{ShaderDataType::Float3, "a_Normal"},
		//		{ShaderDataType::Float2, "a_UV"}
		//	});
		//}
		//	return m_Meshes[0];
	}

	const Ref<Mesh> LoadDefaultQuad()
	{
		//if (m_DefaultQuadLoaded) return m_Meshes[1];
		//else m_DefaultQuadLoaded = true;
		//
		//{
		//	m_Meshes.resize(2);
		//	m_Meshes[1] = std::make_shared<Mesh>();
		//	m_Meshes[1]->SetDataSingularityStatus(true);
		//	m_Meshes[1]->SetSingularMeshData(quadVertices);
		//	m_Meshes[1]->SetVerticesDataLayout
		//	({
		//		{ShaderDataType::Float3, "a_Position"},
		//		{ShaderDataType::Float3, "a_Normal"},
		//		{ShaderDataType::Float2, "a_UV"}
		//	});
		//}
		//	return m_Meshes[1];
	}

	const Ref<Mesh> AssetSystem::LoadDefaultCircle()
	{
		Ref<Mesh> tempMesh = std::make_shared<Mesh>();
		tempMesh->SetDataSingularityStatus(false);
		std::vector<glm::vec3> vertices;
		float cx = 0.0;
		float cy = 0.0;
		uint32_t radius = 1;
		uint32_t num_segments = 50;
		for (uint32_t i = 0; i < num_segments; i++) {
			float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);//get the current angle 
			float x = radius * cosf(theta);//calculate the x component 
			float y = radius * sinf(theta);//calculate the y component 
			vertices.push_back(glm::vec3(x + cx, y + cy, 0.0));
		}
		for (uint32_t i = 0; i < num_segments; i++) {
			float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);//get the current angle 
			float x = radius * cosf(theta);//calculate the x component 
			float y = radius * sinf(theta);//calculate the y component 
			vertices.push_back(glm::vec3(x + cx, 0.0, y + cy));
		}
		tempMesh->SetVertices(vertices);
		m_Meshes.push_back(tempMesh);

		return tempMesh;
	}

	const Ref<Mesh> AssetSystem::LoadDefaultSkybox()
	{
		if (m_DefaultSkyboxLoaded) return m_Meshes[2];
		else m_DefaultSkyboxLoaded = true;

		{
			m_Meshes.resize(3);
			m_Meshes[2] = std::make_shared<Mesh>();
			m_Meshes[2]->SetDataSingularityStatus(true);
			m_Meshes[2]->SetSingularMeshData(skyboxVertices);
			m_Meshes[2]->SetVerticesDataLayout
			({
				{ShaderDataType::Float3, "a_Position"}
			});
		}
		return m_Meshes[2];
	}
#endif
}