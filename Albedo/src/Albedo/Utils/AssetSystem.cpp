#include "AlbedoPreCompiledHeader.h"

#include "AssetSystem.h"
#include "Albedo/Renderer/Buffer.h"

namespace Albedo {

	AssetSystem::AssetSystem()
	{
		LoadDefaultCube();
		LoadDefaultQuad();
		LoadDefaultCircle();
	}

	AssetSystem::~AssetSystem()
	{

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
			if (m->GetPath() == path && m->GetPath() != "")
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

	const Ref<Shader> AssetSystem::LoadShader(const std::string& path)
	{
		Ref<Shader> tempShader = Shader::Create(path);
		tempShader->SetPath(path);
		m_Shaders.push_back(tempShader);

		return tempShader;
	}

	const Ref<Mesh> AssetSystem::LoadDefaultCube()
	{
		if (m_DefaultCubeLoaded) return m_Meshes[0];
		else m_DefaultCubeLoaded = true;

		{
			m_Meshes.resize(1);
			m_Meshes[0] = std::make_shared<Mesh>();
			m_Meshes[0]->SetDataSingularityStatus(true);
			m_Meshes[0]->SetName("Default Cube");
			m_Meshes[0]->SetSingularMeshData(cubeVertices);
			m_Meshes[0]->SetVerticesDataLayout
			({
				{ShaderDataType::Float3, "a_Position"},
				{ShaderDataType::Float3, "a_Normal"},
				{ShaderDataType::Float2, "a_UV"}
			});
		}
			return m_Meshes[0];
	}

	const Ref<Mesh> AssetSystem::LoadDefaultQuad()
	{
		if (m_DefaultQuadLoaded) return m_Meshes[1];
		else m_DefaultQuadLoaded = true;

		{
			m_Meshes.resize(2);
			m_Meshes[1] = std::make_shared<Mesh>();
			m_Meshes[1]->SetDataSingularityStatus(true);
			m_Meshes[1]->SetSingularMeshData(quadVertices);
			m_Meshes[1]->SetVerticesDataLayout
			({
				{ShaderDataType::Float3, "a_Position"},
				{ShaderDataType::Float3, "a_Normal"},
				{ShaderDataType::Float2, "a_TexCoord"}
			});
		}
			return m_Meshes[1];

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
}