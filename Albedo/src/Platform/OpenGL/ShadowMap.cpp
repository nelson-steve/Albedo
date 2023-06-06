#include "AlbedoPreCompiledHeader.h"

#include "ShadowMap.h"

#include <glad/glad.h>

namespace Albedo
{
	ShadowMap::ShadowMap(uint32_t width, uint32_t height)
		:m_Width(width),m_Height(height)
	{
        glGenFramebuffers(1, &m_FramebufferID);

        glGenTextures(1, &m_DepthMapID);
        glBindTexture(GL_TEXTURE_2D, m_DepthMapID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMapID, 0);
        glDrawBuffer(GL_NONE);
        //glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
    void ShadowMap::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
        glViewport(0, 0, m_Width, m_Height);
    }
    void ShadowMap::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}