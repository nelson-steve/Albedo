#pragma once
#if 1

#include "Shader.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "tiny_gltf.h"

#define MAX_NUM_JOINTS 128u

namespace Albedo {
    struct Primitive {
        uint32_t first_index;
        uint32_t index_count;
        uint32_t vertex_count;
        uint32_t material_index;
        Primitive(uint32_t first_index, uint32_t index_count, uint32_t vertex_count, uint32_t mat_index);
    };

    struct material {
        //Texture2D albedo_texture;
    };

    struct GLTF_Mesh {
        std::vector<Primitive*> primitives;
        struct {
            glm::mat4 matrix;
            glm::mat4 joint_matrix[32];
            float joint_count = 0.0f;
        } skin_data;
        GLTF_Mesh(const glm::mat4& mat);
    };

    struct Node;

    struct Node {
        Node* parent;
        uint32_t index;
        std::vector<Node*> children;
        glm::mat4 matrix;
        std::string name;
        GLTF_Mesh* mesh;
        glm::vec3 translation{};
        glm::quat rotation{};
        glm::vec3 scale{ 1.0f };

        const glm::mat4& GetLocalMatrix() {
            return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * matrix;
        }
        const glm::mat4& GetMatrix() {
            glm::mat4 m = GetLocalMatrix();
            Node* p = parent;
            while (p) {
                m = p->GetLocalMatrix() * m;
                p = p->parent;
            }
            return m;
        }
        void Update();
    };

    struct Model {
        //shader* _shader;
        uint32_t vao;
        uint32_t vertices_vbo;
        uint32_t indices_vbo;

        struct vertex {
            glm::vec3 pos;
        };

        struct vertices {
            uint32_t vbo;
        } vertices;

        struct indices {
            uint32_t vbo;
        } indices;

        glm::mat4 aabb;

        std::vector<Node*> nodes;
        std::vector<Node*> linear_nodes;

        //std::vector<Texture2D> textures;
        std::vector<material> materials;

        struct Dimensions {
            glm::vec3 min = glm::vec3(FLT_MAX);
            glm::vec3 max = glm::vec3(-FLT_MAX);
        } dimensions;


        struct loader_info {
            uint32_t* index_buffer;
            vertex* vertex_buffer;
            size_t vertex_pos = 0;
            size_t index_pos = 0;
        };

        tinygltf::Model _model;

        Model() {}
        void Load(const std::string& path);
        void LoadNode(Node* parent, const tinygltf::Node& Node, uint32_t nodeIndex, const tinygltf::Model& Model, loader_info _loader_info);
        void GetNodeProps(const tinygltf::Node& Node, const tinygltf::Model& Model, size_t& vertexCount, size_t& indexCount);
        //void draw_node(Node* _node);
        void draw(Ref<Shader> shader);
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

    };

}
#endif