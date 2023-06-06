#pragma once

namespace Albedo
{
	class ShadowMap
	{
	public:
		ShadowMap(uint32_t width, uint32_t height);

		void Bind();
		void Unbind();

		uint32_t GetDepthMapID() const { return m_DepthMapID; }
	private:
		uint32_t m_Width = 0, m_Height = 0;
		uint32_t m_DepthMapID;
		uint32_t m_FramebufferID;
	};
}