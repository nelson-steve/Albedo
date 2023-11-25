#pragma once

#include "Texture.h"
#include "VertexArray.h"

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

        int16_t index;

        glm::vec4 baseColorFactor;

        float roughnessFactor;
        float metallicFactor;
    };

    struct Primitive {
        uint32_t first_index;
        uint32_t index_count;
        uint32_t vertex_count;
        uint32_t material_index;
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
    };

    class Model {
        struct loader_info;
    public:
        Model() {}
        bool Load(const std::string& path);
        void draw();
    private:
        void LoadNode(Node* parent, const tinygltf::Node& Node, uint32_t nodeIndex, const tinygltf::Model& Model, loader_info _loader_info);
        void GetNodeProps(const tinygltf::Node& Node, const tinygltf::Model& Model, size_t& vertexCount, size_t& indexCount);
        void LoadTextureSamplers(const tinygltf::Model& model);
        void LoadTextures(const tinygltf::Model& model);
        void LoadMaterials(tinygltf::Model& model);
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
        };

        Ref<VertexArray> m_VAO;
        Ref<VertexBuffer> m_VBO;
        Ref<IndexBuffer> m_IBO;
    };

}