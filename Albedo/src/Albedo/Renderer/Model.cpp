#include "AlbedoPreCompiledHeader.h"

#include "Model.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Albedo {
	Primitive::Primitive(uint32_t _first_index, uint32_t _index_count, uint32_t _vertex_count, uint32_t mat_index)
		: first_index(_first_index), index_count(_index_count), vertex_count(_vertex_count), material_index(mat_index) {
	}

	GLTF_Mesh::GLTF_Mesh(const glm::mat4& mat) {
		skin_data.matrix = mat;
	}

	void Model::Load(const std::string& path) {
		//this->_shader = _shader;
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

		if (fileLoaded) {
			_model = gltf_model;
			//load_textures(gltf_model);
			//load_materials(gltf_model);

			const tinygltf::Scene& scene = gltf_model.scenes[gltf_model.defaultScene > -1 ? gltf_model.defaultScene : 0];

			// Get vertex and index buffer sizes up-front
			for (size_t i = 0; i < scene.nodes.size(); i++) {
				GetNodeProps(gltf_model.nodes[scene.nodes[i]], gltf_model, _vertex_count, _index_count);
			}
			_loader_info.vertex_buffer = new vertex[_vertex_count];
			_loader_info.index_buffer = new uint32_t[_index_count];

			// TODO: scene handling with no default scene
			for (size_t i = 0; i < scene.nodes.size(); i++) {
				const tinygltf::Node Node = gltf_model.nodes[scene.nodes[i]];
				LoadNode(nullptr, Node, scene.nodes[i], gltf_model, _loader_info);
			}
		}
		else {
			// TODO: throw
			std::cout << "Could not load gltf file: " << error << std::endl;
			return;
		}

		size_t vertex_buffer_size = _vertex_count * sizeof(vertex);
		size_t index_buffer_size = _index_count * sizeof(uint32_t);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vertices_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, _loader_info.vertex_buffer, GL_STATIC_DRAW);

		auto size = sizeof(_loader_info.index_buffer);
		glGenBuffers(1, &indices_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_size, _loader_info.index_buffer, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		//
		//glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(7 * sizeof(float)));

		//glVertexAttribPointer(vaa, size, accessor.componentType,
		//	accessor.normalized ? GL_TRUE : GL_FALSE,
		//	byteStride, BUFFER_OFFSET(accessor.byteOffset));


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

	void Model::draw(Ref<Shader> shader) {
		glBindVertexArray(vao);
		for (auto& node : nodes) {
			draw_node(node);
		}
	}

	void Model::LoadNode(Node* parent, const tinygltf::Node& gltf_node, uint32_t nodeIndex, const tinygltf::Model& model, loader_info _loader_info) {
		Node* new_node = new Node{};
		new_node->index = nodeIndex;
		new_node->parent = parent;
		new_node->name = gltf_node.name;
		new_node->matrix = glm::mat4(1.0f);

		std::cout << gltf_node.name << std::endl;

		// Generate local Node matrix
		glm::vec3 translation = glm::vec3(0.0f);
		if (gltf_node.translation.size() == 3) {
			translation = glm::make_vec3(gltf_node.translation.data());
			new_node->translation = translation;
		}
		glm::mat4 rotation = glm::mat4(1.0f);
		if (gltf_node.rotation.size() == 4) {
			glm::quat q = glm::make_quat(gltf_node.rotation.data());
			new_node->rotation = glm::mat4(q);
		}
		glm::vec3 scale = glm::vec3(1.0f);
		if (gltf_node.scale.size() == 3) {
			scale = glm::make_vec3(gltf_node.scale.data());
			new_node->scale = scale;
		}
		if (gltf_node.matrix.size() == 16) {
			new_node->matrix = glm::make_mat4x4(gltf_node.matrix.data());
		};

		// Node with children
		if (gltf_node.children.size() > 0) {
			for (size_t i = 0; i < gltf_node.children.size(); i++) {
				LoadNode(new_node, model.nodes[gltf_node.children[i]], gltf_node.children[i], model, _loader_info);
			}
		}

		// Node contains Mesh data
		if (gltf_node.mesh > -1) {
			const tinygltf::Mesh _mesh = model.meshes[gltf_node.mesh];
			GLTF_Mesh* new_mesh = new GLTF_Mesh(new_node->matrix);
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
						vertex& vert = _loader_info.vertex_buffer[_loader_info.vertex_pos];
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