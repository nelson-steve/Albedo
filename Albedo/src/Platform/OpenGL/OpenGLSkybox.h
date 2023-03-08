#pragma once

#include <stdint.h>

namespace Albedo {

	class OpenGLSkybox
	{
	public:
		OpenGLSkybox();
		virtual ~OpenGLSkybox();
	private:
		uint32_t m_SkyboxVAO;
	};

}