#pragma once

#include "AlbedoPreCompiledHeader.h"
#include "Events/Event.h"
#include "Core.h"

namespace Albedo {
	struct WindowProps 
	{
		std::string Title;
		unsigned int Width, Height;
		WindowProps(std::string title = "Albedo Window",
			unsigned int width = 1280, unsigned int height = 720)
			:Title(title), Height(height), Width(width) {}
	};

	class Albedo_API Window 
	{
	public:
		using EventCallBackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual void SetEventCallBack(EventCallBackFn& callback) = 0;
		virtual bool IsVSync() const = 0;

		static Window* Create(const WindowProps props = WindowProps());
	};
}