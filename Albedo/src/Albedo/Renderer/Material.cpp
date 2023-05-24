#include "AlbedoPreCompiledHeader.h"

#include "Material.h"

namespace Albedo {

    void Material::SetAlbedoColor(const glm::vec3& color)
    {
        m_AlbedoColor = color;
    }
    void Material::SetRoughnessScale(float scale)
    {
        m_RoughnessScale = scale;
    }
    void Material::SetExposure(float exposure)
    {
        m_Exposure = exposure;
    }
}