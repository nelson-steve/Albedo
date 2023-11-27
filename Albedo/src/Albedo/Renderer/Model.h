#pragma once

#include "Texture.h"
#include "VertexArray.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tiny_gltf.h>

namespace Albedo {

    struct TextureSampler {
        GLint magFilter;
        GLint minFilter;
        GLenum addressModeS;
        GLenum addressModeT;
        GLenum addressModeW;
    };

    struct GLTF_Material {
        Ref<Texture2D> albedo;
        Ref<Texture2D> metallicRoughness;
        Ref<Texture2D> normal;
        Ref<Texture2D> occlusion;
        Ref<Texture2D> emissive;

        struct TexCoordSets {
            uint8_t albedo = 0;
            uint8_t metallicRoughness = 0;
            uint8_t specularGlossiness = 0;
            uint8_t normal = 0;
            uint8_t occlusion = 0;
            uint8_t emissive = 0;
        } texCoordSets;

        int16_t albedoExists = -1;
        int16_t metallicRoughnessExists = -1;
        int16_t normalExists = -1;
        int16_t occlusionExists = -1;
        int16_t emissiveExists = -1;

        int16_t index;

        glm::vec4 baseColorFactor;

        float roughnessFactor;
        float metallicFactor;
    };

    struct Primitive {
        uint32_t first_index;
        uint32_t index_count;
        uint32_t vertex_count;
        uint32_t vertex_offset;
        int32_t material_index;
        Primitive(uint32_t first_index, uint32_t index_count, uint32_t vertex_count, uint32_t mat_index);
    };

    struct GLTF_Mesh {
        std::vector<Primitive*> primitives;
    };

    struct Node;

    struct Node {
        Node* parent;
        uint32_t index;
        std::vector<Node*> children;
        std::string name;
        GLTF_Mesh* mesh;
    };

    struct Vertex {
        glm::vec3 pos;
        //glm::vec3 normal;
        //glm::vec2 uv0;
        //glm::vec2 uv1;
        //glm::vec4 color;
    };

    class Model {
        struct loader_info;
    public:
        Model() {}
        bool Load(const std::string& path);
        void Draw(Ref<Shader> shader);

        std::vector<GLTF_Material>& GetMaterials() { return m_Materials; }
    private:
        void LoadNode(Node* parent, const tinygltf::Node& Node, uint32_t nodeIndex, const tinygltf::Model& Model, loader_info _loader_info);
        void GetNodeProps(const tinygltf::Node& Node, const tinygltf::Model& Model, size_t& vertexCount, size_t& indexCount);
        void LoadTextureSamplers(const tinygltf::Model& model);
        void LoadTextures(const tinygltf::Model& model);
        void LoadMaterials(tinygltf::Model& model);
        void DrawNode(Node* _node, Ref<Shader> shader);
        //void draw_node(Node* _node);
        //void destroy();
        Node* GetNode(uint32_t index) {
            for (auto& n : linear_nodes) {
                if (n->index == index) {
                    return n;
                }
            }
            assert(false);
        }
        Node* FindNode(Node* parent, uint32_t index);
        Node* NodeFromIndex(uint32_t index);

        std::vector<Node*> nodes;
        std::vector<Node*> linear_nodes;
        std::vector<TextureSampler> m_TextureSamplers;
        std::vector<Ref<Texture2D>> m_Textures;
        std::vector<GLTF_Material> m_Materials;

        struct loader_info {
            uint32_t* index_buffer;
            Vertex* vertex_buffer;
            size_t vertex_pos = 0;
            size_t index_pos = 0;
        } m_LoaderInfo;

        Ref<VertexArray> m_VAO;
        Ref<VertexBuffer> m_VBO;
        Ref<IndexBuffer> m_IBO;

        struct VaoRange
        {
            GLsizei begin; // Index of first element in vertexArrayObjects
            GLsizei count; // Number of elements in range
        };

        tinygltf::Model m_model;
        //std::vector<GLuint> bufferObjects;
        std::vector<GLuint> vertexArrayObjects;
        std::vector<VaoRange> meshToVertexArrays;
    };

}