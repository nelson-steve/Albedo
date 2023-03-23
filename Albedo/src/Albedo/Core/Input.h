#pragma once

#include "Albedo/Core/Core.h"
#include "Albedo/Core/KeyCodes.h"

namespace Albedo {

	class Input
	{
	public:
		inline static bool IsKeyPressed(KeyCode keycode);

		inline static bool IsMouseButtonPressed(int button);
		inline static std::pair<float, float> GetMousePosition();
		inline static float GetMouseX();
		inline static float GetMouseY();
	};

}