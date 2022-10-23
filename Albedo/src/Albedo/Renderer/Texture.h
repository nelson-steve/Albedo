#pragma once

#include <string>
#include <memory>

#include "Albedo/Core.h"


namespace Albedo {

	class Texture
	{
	public:
		~Texture() = default;

		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;

		virtual void Bind(unsigned int slot = 0) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const std::string& path);	
	};

}