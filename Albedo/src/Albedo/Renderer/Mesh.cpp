#include "AlbedoPreCompiledHeader.h"

#include "Renderer.h"
#include "Mesh.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>.

namespace Albedo {

    void Mesh::InitMesh()
    {
        if (m_SingularData)
        {
            m_MeshBufferData.m_VertexArray = VertexArray::Create();
            auto x = sizeof(m_SingularMeshData);
            auto y = GetVertexSize(m_SingularMeshData);
            m_MeshBufferData.m_VertexBuffer = VertexBuffer::Create(m_SingularMeshData, GetVertexSize(m_SingularMeshData));
            m_MeshBufferData.m_VertexBuffer->SetLayout(m_Layout);
            m_MeshBufferData.m_VertexArray->AddVertexBuffer(m_MeshBufferData.m_VertexBuffer);
        }

        else
        {
            m_MeshBufferData.m_VertexArray  = VertexArray::Create();

            if (m_Vertices.size() != 0)
            {
                m_MeshBufferData.m_VertexBuffer = VertexBuffer::Create(m_Vertices, GetVertexSize(m_Vertices));
                m_MeshBufferData.m_VertexBuffer->SetLayout({ {ShaderDataType::Float3, "a_Position"} });
                m_MeshBufferData.m_VertexArray->AddVertexBuffer(m_MeshBufferData.m_VertexBuffer);
            }

            if (m_Normals.size() != 0)
            {
                m_MeshBufferData.m_NormalBuffer = VertexBuffer::Create(m_Normals, GetVertexSize(m_Normals));
                m_MeshBufferData.m_NormalBuffer->SetLayout({ {ShaderDataType::Float3, "a_Normal"} });
                m_MeshBufferData.m_VertexArray->AddVertexBuffer(m_MeshBufferData.m_NormalBuffer);
            }

            if (m_UV.size() != 0)
            {
                m_MeshBufferData.m_UVBuffer = VertexBuffer::Create(m_UV, GetVertexSize(m_UV));
                m_MeshBufferData.m_UVBuffer->SetLayout({ {ShaderDataType::Float2, "a_UV"} });
                m_MeshBufferData.m_VertexArray->AddVertexBuffer(m_MeshBufferData.m_UVBuffer);
            }
        }
    }

}