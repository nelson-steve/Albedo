#pragma once

#include "Albedo/Core/Core.h"

namespace Albedo {

	class Input
	{
	public:
		static bool IsKeyPressed(int keycode);

		inline static bool IsMouseButtonPressed(int button);
		inline static std::pair<float, float> GetMousePosition();
		inline static float GetMouseX();
		inline static float GetMouseY();
	};

}