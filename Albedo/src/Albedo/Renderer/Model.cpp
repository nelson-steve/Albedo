#include "AlbedoPreCompiledHeader.h"

#include "VertexArray.h"
#include "Model.h"
#include "Platform/OpenGL/Utils.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Albedo {

	void Model::LoadTextures(const tinygltf::Model& model) {
		tinygltf::Sampler defaultSampler;
		defaultSampler.minFilter = GL_LINEAR;
		defaultSampler.magFilter = GL_LINEAR;
		defaultSampler.wrapS = GL_REPEAT;
		defaultSampler.wrapT = GL_REPEAT;

		for (size_t i = 0; i < model.textures.size(); ++i) {
			Ref<Texture2D> temp_texture;
			const auto& texture = model.textures[i];
			assert(texture.source >= 0);
			const auto& image = model.images[texture.source];

			const auto& sampler =
				texture.sampler >= 0 ? model.samplers[texture.sampler] : defaultSampler;
			TextureSampler temp_sampler{};
			temp_sampler.minFilter = sampler.minFilter;
			temp_sampler.magFilter = sampler.magFilter;
			temp_sampler.wrapS = sampler.wrapS;
			temp_sampler.wrapT = sampler.wrapT;
			temp_texture = Texture2D::Create(image, temp_sampler);

			m_Textures.push_back(temp_texture);
		}
	}

	bool Model::Load(const std::string& path) {
		m_Path = path;

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

		LoadTextures(gltf_model);
		m_NullTexture = Texture2D::Create(1, 1);
		unsigned char pixels[] = { 0, 0, 0, 0 };
		m_NullTexture->SetData(pixels, Texture::DataType::UNSIGNED_BYTE);

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
	}

	void Model::BindMaterial(Ref<Shader> shader, int materialIndex) {
		if (materialIndex >= 0) {
			const auto& material = m_model.materials[materialIndex];
			const auto& pbrMetallicRoughness = material.pbrMetallicRoughness;
			shader->SetUniformFloat4("u_BaseColorFactor", glm::vec4(
				(float)pbrMetallicRoughness.baseColorFactor[0],
				(float)pbrMetallicRoughness.baseColorFactor[1],
				(float)pbrMetallicRoughness.baseColorFactor[2],
				(float)pbrMetallicRoughness.baseColorFactor[3]));
			shader->SetUniformInt1("u_BaseColorTexture", 0);

			if (pbrMetallicRoughness.baseColorTexture.index >= 0) {
				const auto& texture =
					m_model.textures[pbrMetallicRoughness.baseColorTexture.index];
				if (texture.source >= 0) {
					m_Textures[texture.source]->Bind(0);
					//textureObject = textureObjects[texture.source];
				}
				else {
					m_NullTexture->Bind(0);
				}
			}
			if (pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
				const auto& texture =
					m_model.textures[pbrMetallicRoughness.metallicRoughnessTexture
					.index];
				if (texture.source >= 0) {
					m_Textures[texture.source]->Bind(1);
					//textureObject = textureObjects[texture.source];
				}
				else {
					m_NullTexture->Bind(1);
				}
			}
			if (material.emissiveTexture.index >= 0) {
				const auto& texture = m_model.textures[material.emissiveTexture.index];
				if (texture.source >= 0) {
					m_Textures[texture.source]->Bind(2);
					//textureObject = textureObjects[texture.source];
				}
				else {
					m_NullTexture->Bind(2);
				}
			}
			if (material.occlusionTexture.index >= 0) {
				const auto& texture = m_model.textures[material.occlusionTexture.index];
				if (texture.source >= 0) {
					m_Textures[texture.source]->Bind(3);
					//textureObject = textureObjects[texture.source];
				}
				else {
					m_NullTexture->Bind(3);
				}
			}
			shader->SetUniformFloat("", pbrMetallicRoughness.metallicFactor);
			shader->SetUniformFloat("", pbrMetallicRoughness.roughnessFactor);
			shader->SetUniformFloat3("", glm::vec3(
				(float)material.emissiveFactor[0],
				(float)material.emissiveFactor[1],
				(float)material.emissiveFactor[2]));
			shader->SetUniformFloat("", (float)material.occlusionTexture.strength);
			//if (uMetallicFactor >= 0) {
			//	glUniform1f(
			//		uMetallicFactor, (float)pbrMetallicRoughness.metallicFactor);
			//}
			//if (uRoughnessFactor >= 0) {
			//	glUniform1f(
			//		uRoughnessFactor, (float)pbrMetallicRoughness.roughnessFactor);
			//}
			//if (uEmissiveFactor >= 0) {
			//	glUniform3f(uEmissiveFactor, (float)material.emissiveFactor[0],
			//		(float)material.emissiveFactor[1],
			//		(float)material.emissiveFactor[2]);
			//}
			//if (uOcclusionStrength >= 0) {
			//	glUniform1f(
			//		uOcclusionStrength, (float)material.occlusionTexture.strength);
			//}
		}
		else {
			// Apply default material
			// Defined here:
			// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#reference-material
			// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#reference-pbrmetallicroughness3
			//if (uBaseColorFactor >= 0) {
			//	glUniform4f(uBaseColorFactor, 1, 1, 1, 1);
			//}
			//if (uBaseColorTexture >= 0) {
			//	glActiveTexture(GL_TEXTURE0);
			//	glBindTexture(GL_TEXTURE_2D, whiteTexture);
			//	glUniform1i(uBaseColorTexture, 0);
			//}
			//if (uMetallicFactor >= 0) {
			//	glUniform1f(uMetallicFactor, 1.f);
			//}
			//if (uRoughnessFactor >= 0) {
			//	glUniform1f(uRoughnessFactor, 1.f);
			//}
			//if (uMetallicRoughnessTexture >= 0) {
			//	glActiveTexture(GL_TEXTURE1);
			//	glBindTexture(GL_TEXTURE_2D, 0);
			//	glUniform1i(uMetallicRoughnessTexture, 1);
			//}
			//if (uEmissiveFactor >= 0) {
			//	glUniform3f(uEmissiveFactor, 0.f, 0.f, 0.f);
			//}
			//if (uEmissiveTexture >= 0) {
			//	glActiveTexture(GL_TEXTURE2);
			//	glBindTexture(GL_TEXTURE_2D, 0);
			//	glUniform1i(uEmissiveTexture, 2);
			//}
			//if (uOcclusionStrength >= 0) {
			//	glUniform1f(uOcclusionStrength, 0.f);
			//}
			//if (uOcclusionTexture >= 0) {
			//	glActiveTexture(GL_TEXTURE3);
			//	glBindTexture(GL_TEXTURE_2D, 0);
			//	glUniform1i(uOcclusionTexture, 3);
			//}
		}
		};

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
				const auto& mesh = m_model.meshes[node.mesh];
				const auto& vaoRange = meshToVertexArrays[node.mesh];
				for (size_t pIdx = 0; pIdx < mesh.primitives.size(); ++pIdx) {
					const auto vao = vertexArrayObjects[vaoRange.begin + pIdx];
					const auto& primitive = mesh.primitives[pIdx];

					if(false == m_IsSkybox)
						BindMaterial(shader, primitive.material);

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
	};
}