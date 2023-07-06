#pragma once
#include <string>

namespace Albedo {

	class Image
	{
	public:
		Image();
		Image(const std::string& path);

		void* LoadData() { return data; }

		void LoadImage(const std::string& path);
	
		void* data = nullptr;
		int m_Width = 0;
		int m_Height = 0;
		int m_Channels = 0;
	};

}