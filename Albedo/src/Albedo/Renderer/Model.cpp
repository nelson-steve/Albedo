#include "AlbedoPreCompiledHeader.h"

#include "VertexArray.h"
#include "Model.h"
#include "Platform/OpenGL/Utils.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Albedo {

	Primitive::Primitive(uint32_t _first_index, uint32_t _index_count, uint32_t _vertex_count, uint32_t mat_index)
		: first_index(_first_index), index_count(_index_count), vertex_count(_vertex_count), material_index(mat_index) {
	}

	void Model::LoadTextureSamplers(const tinygltf::Model& model) {
		for (tinygltf::Sampler smpl : model.samplers) {
			TextureSampler sampler{};
			sampler.minFilter = Utils::GetGLFilterMode(smpl.minFilter);
			sampler.magFilter = Utils::GetGLFilterMode(smpl.magFilter);
			sampler.addressModeS = Utils::GetGLWrapMode(smpl.wrapS);
			sampler.addressModeT = Utils::GetGLWrapMode(smpl.wrapT);
			sampler.addressModeW = sampler.addressModeT;
			m_TextureSamplers.push_back(sampler);
		}
	}
	void Model::LoadTextures(const tinygltf::Model& model) {
		for (const tinygltf::Texture& tex : model.textures) {
			tinygltf::Image image = model.images[tex.source];
			TextureSampler sampler;
			if (tex.sampler == -1) {
				// No sampler specified, use a default one
				sampler.magFilter = GL_LINEAR;
				sampler.minFilter = GL_LINEAR;
				sampler.addressModeS = GL_REPEAT;
				sampler.addressModeT = GL_REPEAT;
				sampler.addressModeW = GL_REPEAT;
			}
			else {
				sampler = m_TextureSamplers[tex.sampler];
			}
			Ref<Texture2D> texture;
			texture = Texture2D::Create(image, sampler);
			m_Textures.push_back(texture);
		}
	}

	void Model::LoadMaterials(tinygltf::Model& model) {
		for (tinygltf::Material mat : model.materials) {
			GLTF_Material material{};
			if (mat.values.find("baseColorTexture") != mat.values.end()) {
				material.albedo = m_Textures[mat.values["baseColorTexture"].TextureIndex()]; 
			}
			if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
				material.metallicRoughness = m_Textures[mat.values["metallicRoughnessTexture"].TextureIndex()];
			}
			if (mat.values.find("roughnessFactor") != mat.values.end()) {
				material.roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
			}
			if (mat.values.find("metallicFactor") != mat.values.end()) {
				material.metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
			}
			if (mat.values.find("baseColorFactor") != mat.values.end()) {
				material.baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
			}
			if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
				material.normal = m_Textures[mat.additionalValues["normalTexture"].TextureIndex()];
			}
			if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
				material.emissive = m_Textures[mat.additionalValues["emissiveTexture"].TextureIndex()];
			}
			if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
				material.occlusion = m_Textures[mat.additionalValues["occlusionTexture"].TextureIndex()];
			}

			material.index = static_cast<uint32_t>(m_Materials.size());
			m_Materials.push_back(material);
		}
		//if(model.materials.size() <= 0)
			//m_Materials.push_back(Material());
	}

	bool Model::Load(const std::string& path) {
		tinygltf::Model gltf_model;
		tinygltf::TinyGLTF loader;

		std::string error;
		std::string warning;

		bool binary = false;
		size_t extpos = path.rfind('.', path.length());
		if (extpos != std::string::npos) {
			binary = (path.substr(extpos + 1, path.length() - extpos) == "glb");
		}

		bool fileLoaded = binary ? loader.LoadBinaryFromFile(&gltf_model, &error, &warning, path.c_str()) : loader.LoadASCIIFromFile(&gltf_model, &error, &warning, path.c_str());

		loader_info _loader_info{};
		size_t _vertex_count = 0;
		size_t _index_count = 0;

		if (!fileLoaded) {
			//Albedo_Core_ERROR("Could not load gltf file: ", error);
			std::cout << "Could not load gltf file: " << path << std::endl;
			return false;
		}

		LoadTextureSamplers(gltf_model);
		LoadTextures(gltf_model);
		LoadMaterials(gltf_model);

		const tinygltf::Scene& scene = gltf_model.scenes[gltf_model.defaultScene > -1 ? gltf_model.defaultScene : 0];

		// Get vertex and index buffer sizes up-front
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			GetNodeProps(gltf_model.nodes[scene.nodes[i]], gltf_model, _vertex_count, _index_count);
		}
		_loader_info.vertex_buffer = new Vertex[_vertex_count];
		_loader_info.index_buffer = new uint32_t[_index_count];

		// TODO: scene handling with no default scene
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			const tinygltf::Node Node = gltf_model.nodes[scene.nodes[i]];
			LoadNode(nullptr, Node, scene.nodes[i], gltf_model, _loader_info);
		}

		size_t vertex_buffer_size = _vertex_count * sizeof(Vertex);
		size_t index_buffer_size = _index_count * sizeof(uint32_t);

		m_VAO = VertexArray::Create();

		m_VBO = VertexBuffer::Create(_loader_info.vertex_buffer, vertex_buffer_size);

		BufferLayout layout ({
			{ShaderDataType::Float3, "a_Position"},
			});

		m_VBO->SetLayout(layout);
		m_VAO->AddVertexBuffer(m_VBO);

		assert(vertex_buffer_size > 0);

		delete[] _loader_info.vertex_buffer;
		delete[] _loader_info.index_buffer;
	}

	void draw_node(Node* _node)
	{
		if (_node->mesh) {
			for (Primitive* _primitive : _node->mesh->primitives) {
				//uint32_t end = _primitive->first_index - _primitive->index_count;
				//glDrawRangeElements(GL_TRIANGLES, _primitive->first_index, _primitive->index_count, _primitive->index_count, GL_UNSIGNED_BYTE, 0);
				//glDrawElements(GL_TRIANGLES, _primitive->index_count, GL_UNSIGNED_INT, 0);
				glDrawArrays(GL_TRIANGLES, 0, _primitive->vertex_count);
			}
		}
		for (auto& child : _node->children) {
			draw_node(child);
		}
	}

	void Model::draw() {
		m_VAO->Bind();
		for (auto& node : nodes) {
			draw_node(node);
		}
	}

	void Model::LoadNode(Node* parent, const tinygltf::Node& gltf_node, uint32_t nodeIndex, const tinygltf::Model& model, loader_info _loader_info) {
		Node* new_node = new Node{};
		new_node->index = nodeIndex;
		new_node->parent = parent;
		new_node->name = gltf_node.name;

		// Node with children
		if (gltf_node.children.size() > 0) {
			for (size_t i = 0; i < gltf_node.children.size(); i++) {
				LoadNode(new_node, model.nodes[gltf_node.children[i]], gltf_node.children[i], model, _loader_info);
			}
		}

		// Node contains Mesh data
		if (gltf_node.mesh > -1) {
			const tinygltf::Mesh _mesh = model.meshes[gltf_node.mesh];
			GLTF_Mesh* new_mesh = new GLTF_Mesh();
			for (size_t j = 0; j < _mesh.primitives.size(); j++) {
				const tinygltf::Primitive& _primitive = _mesh.primitives[j];
				uint32_t vertexStart = static_cast<uint32_t>(_loader_info.vertex_pos);
				uint32_t indexStart = static_cast<uint32_t>(_loader_info.index_pos);
				uint32_t indexCount = 0;
				uint32_t vertexCount = 0;
				glm::vec3 posMin{};
				glm::vec3 posMax{};
				bool hasSkin = false;
				bool hasIndices = _primitive.indices > -1;
				// Vertices
				{
					const float* bufferPos = nullptr;
					const void* bufferJoints = nullptr;
					const float* bufferWeights = nullptr;

					int posByteStride;
					int jointByteStride;
					int weightByteStride;

					int jointComponentType;

					// Position attribute is required
					assert(_primitive.attributes.find("POSITION") != _primitive.attributes.end());

					const tinygltf::Accessor& posAccessor = model.accessors[_primitive.attributes.find("POSITION")->second];
					const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
					bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
					posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
					posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
					vertexCount = static_cast<uint32_t>(posAccessor.count);
					posByteStride = posAccessor.ByteStride(posView) ? (posAccessor.ByteStride(posView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

					for (size_t v = 0; v < posAccessor.count; v++) {
						Vertex& vert = _loader_info.vertex_buffer[_loader_info.vertex_pos];
						vert.pos = glm::vec4(glm::make_vec3(&bufferPos[v * posByteStride]), 1.0f);

						_loader_info.vertex_pos++;
					}
				}
				// Indices
				if (hasIndices)
				{
					const tinygltf::Accessor& accessor = model.accessors[_primitive.indices > -1 ? _primitive.indices : 0];
					const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

					indexCount = static_cast<uint32_t>(accessor.count);
					const void* dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);

					switch (accessor.componentType) {
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
						const uint32_t* buf = static_cast<const uint32_t*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++) {
							_loader_info.index_buffer[_loader_info.index_pos] = buf[index] + vertexStart;
							_loader_info.index_pos++;
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
						const uint16_t* buf = static_cast<const uint16_t*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++) {
							_loader_info.index_buffer[_loader_info.index_pos] = buf[index] + vertexStart;
							_loader_info.index_pos++;
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
						const uint8_t* buf = static_cast<const uint8_t*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++) {
							_loader_info.index_buffer[_loader_info.index_pos] = buf[index] + vertexStart;
							_loader_info.index_pos++;
						}
						break;
					}
					default:
						std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
						return;
					}
				}
				Primitive* new_primitive = new Primitive(indexStart, indexCount, vertexCount, _primitive.material > -1 ? _primitive.material : -1);
				new_mesh->primitives.push_back(new_primitive);
			}
			new_node->mesh = new_mesh;
		}
		if (parent) {
			parent->children.push_back(new_node);
		}
		else {
			nodes.push_back(new_node);
		}
		linear_nodes.push_back(new_node);
	}

	void Model::GetNodeProps(const tinygltf::Node& node, const tinygltf::Model& model, size_t& vertexCount, size_t& indexCount) {
		if (node.children.size() > 0) {
			for (size_t i = 0; i < node.children.size(); i++) {
				GetNodeProps(model.nodes[node.children[i]], model, vertexCount, indexCount);
			}
		}
		if (node.mesh > -1) {
			const tinygltf::Mesh Mesh = model.meshes[node.mesh];
			for (size_t i = 0; i < Mesh.primitives.size(); i++) {
				auto Primitive = Mesh.primitives[i];
				vertexCount += model.accessors[Primitive.attributes.find("POSITION")->second].count;
				if (Primitive.indices > -1) {
					indexCount += model.accessors[Primitive.indices].count;
				}
			}
		}
	}

	Node* Model::FindNode(Node* parent, uint32_t index) {
		Node* node_found = nullptr;
		if (parent->index == index) {
			return parent;
		}
		for (auto& child : parent->children) {
			node_found = FindNode(child, index);
			if (node_found) {
				break;
			}
		}
		return node_found;
	}

	Node* Model::NodeFromIndex(uint32_t index) {
		Node* node_found = nullptr;
		for (auto& node : nodes) {
			node_found = FindNode(node, index);
			if (node_found) {
				break;
			}
		}
		return node_found;
	}
}