#pragma once

#include <string>
#include <memory>

#include "Albedo/Core/Core.h"


namespace Albedo {

	class Texture
	{
	public:
		~Texture() = default;

		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;

		virtual void SetData(void* data, unsigned int size) = 0;

		virtual void Bind() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const std::string& path);
		static Ref<Texture2D> Create(unsigned int width, unsigned int height);
	};

}