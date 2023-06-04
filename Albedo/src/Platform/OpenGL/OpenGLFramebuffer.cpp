#include "AlbedoPreCompiledHeader.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>
#include "Utils.h"

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

		static void AttachColorTexture(uint32_t id, GLenum internalFormat, GLenum format, const FramebufferSpecification& spec, const TextureConfiguration& config, int index)
		{
			GLenum dataFormat = AlbedoToOpenGLENUMType<Config::DataFormat>(config.m_DataFormat);
			GLenum texLayout  = AlbedoToOpenGLENUMType<Config::TextureLayout>(config.m_TextureLayout);
			GLenum dataType   = AlbedoToOpenGLENUMType<Config::DataType>(config.m_DataType);
			GLenum texType    = AlbedoToOpenGLENUMType<Config::TextureType>(config.m_TextureType);

			GLint inFormat    = AlbedoToOpenGLINTType<Config::InternalFormat>(config.m_InternalFormat);
			GLint minFilter   = AlbedoToOpenGLINTType<Config::MinMagFilters>(config.m_MinFilter);
			GLint magFilter   = AlbedoToOpenGLINTType<Config::MinMagFilters>(config.m_MagFilter);

			bool multisampled = spec.Samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, internalFormat, spec.Width, spec.Height, GL_FALSE);
			}
			else
			{
				//if (format == GL_RED_INTEGER)
				//{
				//	int value = -1;
				//	glTexImage2D(GL_TEXTURE_2D, 0, GL_INT, 1, 1, 0, format, GL_INT, (void *) & value);
				//}
				glTexImage2D(texType, 0, internalFormat, spec.Width, spec.Height, 0, format, dataType, nullptr);

				glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, minFilter);
				glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, magFilter);
				glTexParameteri(texType, GL_TEXTURE_WRAP_R, texLayout);
				glTexParameteri(texType, GL_TEXTURE_WRAP_S, texLayout);
				glTexParameteri(texType, GL_TEXTURE_WRAP_T, texLayout);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, GLenum internalFormat, GLenum format, GLenum attachmentType, const FramebufferSpecification& spec, const TextureConfiguration& config)
		{
			GLenum dataFormat = AlbedoToOpenGLENUMType<Config::DataFormat>(config.m_DataFormat);
			GLenum texLayout = AlbedoToOpenGLENUMType<Config::TextureLayout>(config.m_TextureLayout);
			GLenum dataType = AlbedoToOpenGLENUMType<Config::DataType>(config.m_DataType);
			GLenum texType = AlbedoToOpenGLENUMType<Config::TextureType>(config.m_TextureType);

			GLint inFormat = AlbedoToOpenGLINTType<Config::InternalFormat>(config.m_InternalFormat);
			GLint minFilter = AlbedoToOpenGLINTType<Config::MinMagFilters>(config.m_MinFilter);
			GLint magFilter = AlbedoToOpenGLINTType<Config::MinMagFilters>(config.m_MagFilter);

			bool multisampled = spec.Samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, format, spec.Width, spec.Height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, spec.Width, spec.Height, 0, format, GL_FLOAT, NULL);
				//glTexStorage2D(texType, 1, format, spec.Width, spec.Height);

				glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, minFilter);
				glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, magFilter);
				glTexParameteri(texType, GL_TEXTURE_WRAP_R, texLayout);
				glTexParameteri(texType, GL_TEXTURE_WRAP_S, texLayout);
				glTexParameteri(texType, GL_TEXTURE_WRAP_T, texLayout);

				if (spec.s_BorderColor)
				{
					float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
					glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
				}
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static void AttachRenderbuffer(uint32_t& id, const FramebufferSpecification& spec, const TextureConfiguration& config)
		{
			glGenRenderbuffers(1, &id);
			glBindRenderbuffer(GL_RENDERBUFFER, id);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, spec.Width, spec.Height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id);

		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
				case FramebufferTextureFormat::Depth:  return true;
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

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec, const TextureConfiguration& config)
		: m_Specification(spec), m_TextureConfiguration(config)
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
			m_RenderbufferAttachment = 0;
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
						Utils::AttachColorTexture(m_ColorAttachments[i], GL_RGBA8, GL_RGBA, m_Specification, m_TextureConfiguration, i);
						break;
					case FramebufferTextureFormat::RED_INTEGER:
						Utils::AttachColorTexture(m_ColorAttachments[i], GL_R32I, GL_RED_INTEGER, m_Specification, m_TextureConfiguration, i);
						//Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
						break;
				}
			}
		}

		if (Utils::IsDepthFormat(m_DepthAttachmentSpecification.TextureFormat))
		{
			Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
			Utils::BindTexture(multisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					Utils::AttachDepthTexture(m_DepthAttachment, GL_DEPTH24_STENCIL8, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, m_Specification, m_TextureConfiguration);
					break;
				case FramebufferTextureFormat::Depth:
					Utils::AttachDepthTexture(m_DepthAttachment, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, m_Specification, m_TextureConfiguration);
					break;
			}
		}

		if (m_RenderbufferAttachmentSpecification.TextureFormat == FramebufferTextureFormat::RENDER_BUFFER)
		{
			switch (m_RenderbufferAttachmentSpecification.TextureFormat)
			{
			case FramebufferTextureFormat::RENDER_BUFFER:
				Utils::AttachRenderbuffer(m_RenderbufferAttachment, m_Specification, m_TextureConfiguration);
				break;
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			Albedo_CORE_ASSERT(m_ColorAttachments.size() <= 4, "error");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (!m_Specification.s_DrawBuffer || m_ColorAttachments.empty() && m_RenderbufferAttachmentSpecification.TextureFormat != FramebufferTextureFormat::RENDER_BUFFER)
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}

		if (!m_Specification.s_ReadBuffer)
			glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			Albedo_Core_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
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