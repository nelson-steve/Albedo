#include "AlbedoPreCompiledHeader.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Albedo {

	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace Utils {

		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glGenTextures(count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				//if (format == GL_RED_INTEGER)
				//{
				//	int value = -1;
				//	glTexImage2D(GL_TEXTURE_2D, 0, GL_INT, 1, 1, 0, format, GL_INT, (void *) & value);
				//}
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static void AttachRenderbuffer(uint32_t id, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			glGenRenderbuffers(1, &id);
			glBindRenderbuffer(GL_RENDERBUFFER, id);
			glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, id);
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
			}

			return false;
		}

		static bool IsColorFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::RGBA8:		 return true;
				case FramebufferTextureFormat::RED_INTEGER:  return true;
			}

			return false;
		}

		static GLenum AlbedoFBTextureFormatToGL(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
				case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}

			Albedo_CORE_ASSERT(false, "error");
			return 0;
		}


	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (Utils::IsColorFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else if (Utils::IsDepthFormat(spec.TextureFormat))
				m_DepthAttachmentSpecification = spec;
			else if (spec.TextureFormat == FramebufferTextureFormat::RENDER_BUFFER)
				m_RenderbufferAttachmentSpecification = spec;
			else
				Albedo_Core_ERROR("Unknown Frame buffer texture format");
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_FramebufferID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_FramebufferID)
		{
			glDeleteFramebuffers(1, &m_FramebufferID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}
		glGenFramebuffers(1, &m_FramebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

		bool multisample = m_Specification.Samples > 1;

		// Attachments
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				Utils::BindTexture(multisample, m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
					case FramebufferTextureFormat::RGBA8:
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						break;
					case FramebufferTextureFormat::RED_INTEGER:
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
						break;
				}
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat == FramebufferTextureFormat::DEPTH24STENCIL8)
		{
			Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
			Utils::BindTexture(multisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
			}
		}

		if (m_RenderbufferAttachmentSpecification.TextureFormat == FramebufferTextureFormat::RENDER_BUFFER)
		{
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
			case FramebufferTextureFormat::RENDER_BUFFER:
				Utils::AttachRenderbuffer(m_RenderbufferAttachment, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			Albedo_CORE_ASSERT(m_ColorAttachments.size() <= 4, "error");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			Albedo_Core_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);

		//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		//	Albedo_Core_INFO("Framebuffer complete");
		//else
		//	Albedo_Core_ERROR("WARNING: Framebuffer incomplete");
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			Albedo_Core_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		Albedo_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "ERROR: Index out of range");

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		//glClearTexImage(m_ColorAttachments[attachmentIndex], 0, Utils::AlbedoFBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
		glClearBufferiv(GL_COLOR, attachmentIndex, &value);
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		Albedo_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "error");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;

	}

}