#pragma once

#include "Albedo/Layers/Layer.h"
#include "Albedo/Core.h"

#include "Albedo/Events/ApplicationEvent.h"
#include "Albedo/Events/KeyEvent.h"
#include "Albedo/Events/MouseEvent.h"

namespace Albedo {

	class Albedo_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};

}