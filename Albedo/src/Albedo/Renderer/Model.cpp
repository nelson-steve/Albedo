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
				material.texCoordSets.albedo = mat.values["baseColorTexture"].TextureTexCoord();
				material.albedoExists = 1;
			}
			if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
				material.metallicRoughness = m_Textures[mat.values["metallicRoughnessTexture"].TextureIndex()];
				material.texCoordSets.metallicRoughness = mat.values["metallicRoughnessTexture"].TextureTexCoord();
				material.metallicRoughnessExists = 1;
			}
			if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
				material.normal = m_Textures[mat.additionalValues["normalTexture"].TextureIndex()];
				material.texCoordSets.normal = mat.additionalValues["normalTexture"].TextureTexCoord();
				material.normalExists = 1;
			}
			if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
				material.emissive = m_Textures[mat.additionalValues["emissiveTexture"].TextureIndex()];
				material.texCoordSets.emissive = mat.additionalValues["emissiveTexture"].TextureTexCoord();
				material.emissiveExists = 1;
			}
			if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
				material.occlusion = m_Textures[mat.additionalValues["occlusionTexture"].TextureIndex()];
				material.texCoordSets.occlusion = mat.additionalValues["occlusionTexture"].TextureTexCoord();
				material.albedoExists = 1;
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

			material.index = static_cast<uint32_t>(m_Materials.size());
			m_Materials.push_back(material);
		}
		//m_Materials.push_back(GLTF_Material());
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

		size_t _vertex_count = 0;
		size_t _index_count = 0;

		if (!fileLoaded) {
			//Albedo_Core_ERROR("Could not load gltf file: ", error);
			std::cout << "Could not load gltf file: " << path << std::endl;
			return false;
		}
		m_model = gltf_model;

		//std::vector<GLuint> bufferObjects(gltf_model.buffers.size(), 0);
		std::vector<GLuint> bufferObjects(gltf_model.buffers.size(), 0);

		glGenBuffers(GLsizei(gltf_model.buffers.size()), bufferObjects.data());
		for (size_t i = 0; i < gltf_model.buffers.size(); ++i) {
			glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[i]);
			glBufferData(GL_ARRAY_BUFFER, gltf_model.buffers[i].data.size(),
				gltf_model.buffers[i].data.data(), GL_STATIC_DRAW);
			//glBufferStorage(GL_ARRAY_BUFFER, gltf_model.buffers[i].data.size(),
			//	gltf_model.buffers[i].data.data(), 0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//std::vector<GLuint> vertexArrayObjects; // We don't know the size yet
		vertexArrayObjects; // We don't know the size yet

		//std::vector<VaoRange> meshToVertexArrays;
		// For each mesh of model we keep its range of VAOs
		meshToVertexArrays.resize(gltf_model.meshes.size());

		const GLuint VERTEX_ATTRIB_POSITION_IDX = 0;
		const GLuint VERTEX_ATTRIB_NORMAL_IDX = 1;
		const GLuint VERTEX_ATTRIB_TEXCOORD0_IDX = 2;

		for (size_t i = 0; i < gltf_model.meshes.size(); ++i) {
			const auto& mesh = gltf_model.meshes[i];

			auto& vaoRange = meshToVertexArrays[i];
			vaoRange.begin =
				GLsizei(vertexArrayObjects.size()); // Range for this mesh will be at
			// the end of vertexArrayObjects
			vaoRange.count =
				GLsizei(mesh.primitives.size()); // One VAO for each primitive

			// Add enough elements to store our VAOs identifiers
			vertexArrayObjects.resize(
				vertexArrayObjects.size() + mesh.primitives.size());

			glGenVertexArrays(vaoRange.count, &vertexArrayObjects[vaoRange.begin]);
			for (size_t pIdx = 0; pIdx < mesh.primitives.size(); ++pIdx) {
				const auto vao = vertexArrayObjects[vaoRange.begin + pIdx];
				const auto& primitive = mesh.primitives[pIdx];
				glBindVertexArray(vao);
				{ // POSITION attribute
				  // scope, so we can declare const variable with the same name on each
				  // scope
					const auto iterator = primitive.attributes.find("POSITION");
					if (iterator != end(primitive.attributes)) {
						const auto accessorIdx = (*iterator).second;
						const auto& accessor = gltf_model.accessors[accessorIdx];
						const auto& bufferView = gltf_model.bufferViews[accessor.bufferView];
						const auto bufferIdx = bufferView.buffer;

						glEnableVertexAttribArray(VERTEX_ATTRIB_POSITION_IDX);
						assert(GL_ARRAY_BUFFER == bufferView.target);
						// Theorically we could also use bufferView.target, but it is safer
						// Here it is important to know that the next call
						// (glVertexAttribPointer) use what is currently bound
						glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[bufferIdx]);

						// tinygltf converts strings type like "VEC3, "VEC2" to the number of
						// components, stored in accessor.type
						const auto byteOffset = accessor.byteOffset + bufferView.byteOffset;
						glVertexAttribPointer(VERTEX_ATTRIB_POSITION_IDX, accessor.type,
							accessor.componentType, GL_FALSE, GLsizei(bufferView.byteStride),
							(const GLvoid*)byteOffset);
					}
				}
				// todo Refactor to remove code duplication (loop over "POSITION",
				// "NORMAL" and their corresponding VERTEX_ATTRIB_*)
				{ // NORMAL attribute
					const auto iterator = primitive.attributes.find("NORMAL");
					if (iterator != end(primitive.attributes)) {
						const auto accessorIdx = (*iterator).second;
						const auto& accessor = gltf_model.accessors[accessorIdx];
						const auto& bufferView = gltf_model.bufferViews[accessor.bufferView];
						const auto bufferIdx = bufferView.buffer;

						glEnableVertexAttribArray(VERTEX_ATTRIB_NORMAL_IDX);
						assert(GL_ARRAY_BUFFER == bufferView.target);
						glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[bufferIdx]);
						glVertexAttribPointer(VERTEX_ATTRIB_NORMAL_IDX, accessor.type,
							accessor.componentType, GL_FALSE, GLsizei(bufferView.byteStride),
							(const GLvoid*)(accessor.byteOffset + bufferView.byteOffset));
					}
				}
				{ // TEXCOORD_0 attribute
					const auto iterator = primitive.attributes.find("TEXCOORD_0");
					if (iterator != end(primitive.attributes)) {
						const auto accessorIdx = (*iterator).second;
						const auto& accessor = gltf_model.accessors[accessorIdx];
						const auto& bufferView = gltf_model.bufferViews[accessor.bufferView];
						const auto bufferIdx = bufferView.buffer;

						glEnableVertexAttribArray(VERTEX_ATTRIB_TEXCOORD0_IDX);
						assert(GL_ARRAY_BUFFER == bufferView.target);
						glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[bufferIdx]);
						glVertexAttribPointer(VERTEX_ATTRIB_TEXCOORD0_IDX, accessor.type,
							accessor.componentType, GL_FALSE, GLsizei(bufferView.byteStride),
							(const GLvoid*)(accessor.byteOffset + bufferView.byteOffset));
					}
				}
				// Index array if defined
				if (primitive.indices >= 0) {
					const auto accessorIdx = primitive.indices;
					const auto& accessor = gltf_model.accessors[accessorIdx];
					const auto& bufferView = gltf_model.bufferViews[accessor.bufferView];
					const auto bufferIdx = bufferView.buffer;

					assert(GL_ELEMENT_ARRAY_BUFFER == bufferView.target);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
						bufferObjects[bufferIdx]); // Binding the index buffer to
					// GL_ELEMENT_ARRAY_BUFFER while the VAO
					// is bound is enough to tell OpenGL we
					// want to use that index buffer for that
					// VAO
				}
			}
		}
		glBindVertexArray(0);

		// end
		return true;

		LoadTextureSamplers(gltf_model);
		LoadTextures(gltf_model);
		LoadMaterials(gltf_model);

		const tinygltf::Scene& scene = gltf_model.scenes[gltf_model.defaultScene > -1 ? gltf_model.defaultScene : 0];

		// Get vertex and index buffer sizes up-front
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			GetNodeProps(gltf_model.nodes[scene.nodes[i]], gltf_model, _vertex_count, _index_count);
		}
		m_LoaderInfo.vertex_buffer = new Vertex[_vertex_count];
		m_LoaderInfo.index_buffer = new uint32_t[_index_count];

		// TODO: scene handling with no default scene
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			const tinygltf::Node Node = gltf_model.nodes[scene.nodes[i]];
			LoadNode(nullptr, Node, scene.nodes[i], gltf_model, m_LoaderInfo);
		}

		size_t vertex_buffer_size = _vertex_count * sizeof(Vertex);
		size_t index_buffer_size = _index_count * sizeof(uint32_t);

		m_VAO = VertexArray::Create();

		m_VBO = VertexBuffer::Create(m_LoaderInfo.vertex_buffer, vertex_buffer_size);

		BufferLayout layout ({
			{ShaderDataType::Float3, "a_Position"},
			//{ShaderDataType::Float3, "a_Normal"},
			//{ShaderDataType::Float2, "a_UV0"},
			//{ShaderDataType::Float2, "a_UV1"},
			//{ShaderDataType::Float4, "a_Color"},
			});

		m_VBO->SetLayout(layout);
		m_VAO->AddVertexBuffer(m_VBO);

		assert(vertex_buffer_size > 0);

		//delete[] _loader_info.vertex_buffer;
		//delete[] _loader_info.index_buffer;
	}

	void Model::DrawNode(Node* _node, Ref<Shader> shader)
	{
		shader->Bind();
		if (_node->mesh) {
			for (Primitive* primitive : _node->mesh->primitives) {
				GLTF_Material material;
				if (primitive->material_index > -1)
					material = m_Materials[primitive->material_index];
				else
					material = m_Materials.back();
				//if(material.albedoExists > -1) 
					material.albedo->Bind(0);
				//if(material.metallicRoughnessExists > -1) 
					material.metallicRoughness->Bind(1);
				//if(material.normalExists > -1) 
					material.normal->Bind(2);
				//if(material.occlusionExists > -1) 
					material.occlusion->Bind(3);
				//if(material.emissiveExists > -1) 
					material.emissive->Bind(4);

				shader->SetUniformInt1("u_AlbedoExists", material.albedoExists);
				shader->SetUniformInt1("u_MetallicRoughnessExists", material.metallicRoughnessExists);
				shader->SetUniformInt1("u_NormalExists", material.normalExists);
				shader->SetUniformInt1("u_OcclusionExists", material.occlusionExists);
				shader->SetUniformInt1("u_EmissiveExists", material.emissiveExists);

				if (material.albedoExists > -1)
					shader->SetUniformInt1("u_AlbedoSet", material.texCoordSets.albedo);
				if (material.metallicRoughnessExists > -1)
					shader->SetUniformInt1("u_MetallicRoughnessSet", material.texCoordSets.metallicRoughness);
				if (material.normalExists > -1)
					shader->SetUniformInt1("u_NormalSet", material.texCoordSets.normal);
				if (material.occlusionExists > -1)
					shader->SetUniformInt1("u_OcclusionSet", material.texCoordSets.occlusion);
				if (material.emissiveExists > -1)
					shader->SetUniformInt1("u_EmissiveSet", material.texCoordSets.emissive);

				//glDrawArrays(GL_TRIANGLES, 0, primitive->vertex_count);
				//glDrawElementsBaseVertex(GL_TRIANGLES, primitive->index_count, GL_UNSIGNED_INT,
					//m_LoaderInfo.index_buffer, primitive->index_count);
				//glDrawRangeElements(GL_TRIANGLES, primitive->first_index, 2, primitive->index_count, GL_UNSIGNED_INT, m_LoaderInfo.index_buffer);
				glDrawElements(GL_TRIANGLES, primitive->index_count, GL_UNSIGNED_INT, m_LoaderInfo.index_buffer);
				//glDrawArraysInstanced(GL_TRIANGLES, primitive->first_index, primitive->index_count, 1);
				//glDrawRangeElements(GL_TRIANGLES, primitive->first_index, primitive->index_count, )
			}
		}
		for (auto& child : _node->children) {
			DrawNode(child, shader);
		}
	}

	void Model::Draw(Ref<Shader> shader) {
		// The recursive function that should draw a node
   // We use a std::function because a simple lambda cannot be recursive
		const std::function<void(int, const glm::mat4&)> drawNode =
			[&](int nodeIdx, const glm::mat4& parentMatrix) {
			const auto& node = m_model.nodes[nodeIdx];
			const glm::mat4 modelMatrix = glm::mat4(1.0f);
				//getLocalToWorldMatrix(node, parentMatrix);

			// If the node references a mesh (a node can also reference a
			// camera, or a light)
			if (node.mesh >= 0) {
				//const auto mvMatrix =
				//	camera->GetViewMatrix() * modelMatrix; // Also called localToCamera matrix
				//const auto mvpMatrix =
				//	camera->GetProjection() * mvMatrix; // Also called localToScreen matrix
				// Normal matrix is necessary to maintain normal vectors
				// orthogonal to tangent vectors
				// https://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/
				//const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

				//glUniformMatrix4fv(modelViewProjMatrixLocation, 1, GL_FALSE,
				//	glm::value_ptr(mvpMatrix));
				//glUniformMatrix4fv(
				//	modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
				//glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE,
				//	glm::value_ptr(normalMatrix));

				const auto& mesh = m_model.meshes[node.mesh];
				const auto& vaoRange = meshToVertexArrays[node.mesh];
				for (size_t pIdx = 0; pIdx < mesh.primitives.size(); ++pIdx) {
					const auto vao = vertexArrayObjects[vaoRange.begin + pIdx];
					const auto& primitive = mesh.primitives[pIdx];

					//bindMaterial(primitive.material);

					glBindVertexArray(vao);
					if (primitive.indices >= 0) {
						const auto& accessor = m_model.accessors[primitive.indices];
						const auto& bufferView = m_model.bufferViews[accessor.bufferView];
						const auto byteOffset =
							accessor.byteOffset + bufferView.byteOffset;
						glDrawElements(primitive.mode, GLsizei(accessor.count),
							accessor.componentType, (const GLvoid*)byteOffset);
					}
					else {
						// Take first accessor to get the count
						const auto accessorIdx = (*begin(primitive.attributes)).second;
						const auto& accessor = m_model.accessors[accessorIdx];
						glDrawArrays(primitive.mode, 0, GLsizei(accessor.count));
					}
				}
			}

			// Draw children
			for (const auto childNodeIdx : node.children) {
				drawNode(childNodeIdx, modelMatrix);
			}
			};

		// Draw the scene referenced by gltf file
		if (m_model.defaultScene >= 0) {
			for (const auto nodeIdx : m_model.scenes[m_model.defaultScene].nodes) {
				drawNode(nodeIdx, glm::mat4(1));
			}
		}

		// end
		return;

		m_VAO->Bind();
		for (auto& node : nodes) {
			DrawNode(node, shader);
		}
	}

	void Model::LoadNode(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, loader_info _loader_info) {
		Node* new_node = new Node{};
		new_node->index = nodeIndex;
		new_node->parent = parent;
		new_node->name = node.name;

		// Node with children
		if (node.children.size() > 0) {
			for (size_t i = 0; i < node.children.size(); i++) {
				LoadNode(new_node, model.nodes[node.children[i]], node.children[i], model, _loader_info);
			}
		}

		if (node.mesh > -1) {
			const tinygltf::Mesh mesh = model.meshes[node.mesh];
			GLTF_Mesh* newMesh = new GLTF_Mesh();
			for (size_t j = 0; j < mesh.primitives.size(); j++) {
				const tinygltf::Primitive& primitive = mesh.primitives[j];
				uint32_t vertexStart = static_cast<uint32_t>(_loader_info.vertex_pos);
				uint32_t indexStart = static_cast<uint32_t>(_loader_info.index_pos);
				uint32_t indexCount = 0;
				uint32_t vertexCount = 0;
				glm::vec3 posMin{};
				glm::vec3 posMax{};
				bool hasSkin = false;
				bool hasIndices = primitive.indices > -1;
				// Vertices
				{
					const float* bufferPos = nullptr;
					const float* bufferNormals = nullptr;
					const float* bufferTexCoordSet0 = nullptr;
					const float* bufferTexCoordSet1 = nullptr;

					int posByteStride;
					int normByteStride;
					int uv0ByteStride;
					int uv1ByteStride;

					// Position attribute is required
					assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

					const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
					const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
					bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
					posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
					posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
					vertexCount = static_cast<uint32_t>(posAccessor.count);
					posByteStride = posAccessor.ByteStride(posView) ? (posAccessor.ByteStride(posView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

					if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
						const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
						const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
						bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
						normByteStride = normAccessor.ByteStride(normView) ? (normAccessor.ByteStride(normView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
					}

					// UVs
					if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
						const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
						const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
						bufferTexCoordSet0 = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
						uv0ByteStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
					}
					if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end()) {
						const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_1")->second];
						const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
						bufferTexCoordSet1 = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
						uv1ByteStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
					}

					for (size_t v = 0; v < posAccessor.count; v++) {
						Vertex& vert = _loader_info.vertex_buffer[_loader_info.vertex_pos];
						vert.pos = glm::vec4(glm::make_vec3(&bufferPos[v * posByteStride]), 1.0f);
						//vert.normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * normByteStride]) : glm::vec3(0.0f)));
						//vert.uv0 = bufferTexCoordSet0 ? glm::make_vec2(&bufferTexCoordSet0[v * uv0ByteStride]) : glm::vec3(0.0f);
						//vert.uv1 = bufferTexCoordSet1 ? glm::make_vec2(&bufferTexCoordSet1[v * uv1ByteStride]) : glm::vec3(0.0f);
						//vert.color = bufferColorSet0 ? glm::make_vec4(&bufferColorSet0[v * color0ByteStride]) : glm::vec4(1.0f);

						_loader_info.vertex_pos++;
					}
				}
				// Indices
				if (hasIndices)
				{
					const tinygltf::Accessor& accessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
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
				Primitive* newPrimitive = new Primitive(indexStart, indexCount, vertexCount, primitive.material > -1 ? primitive.material : -1);
				newMesh->primitives.push_back(newPrimitive);
			}
			new_node->mesh = newMesh;
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
			const tinygltf::Mesh mesh = model.meshes[node.mesh];
			for (size_t i = 0; i < mesh.primitives.size(); i++) {
				auto primitive = mesh.primitives[i];
				vertexCount += model.accessors[primitive.attributes.find("POSITION")->second].count;
				if (primitive.indices > -1) {
					indexCount += model.accessors[primitive.indices].count;
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