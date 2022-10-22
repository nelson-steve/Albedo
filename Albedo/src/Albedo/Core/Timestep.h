#pragma once

namespace Albedo {
	
	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
			:m_Time(time)
		{

		}

		operator float() const { return m_Time; }

		float GetTime() const { return m_Time; }
		float GetTimeMillis() { return m_Time * 1000.0f; }
	private:
		float m_Time;
	};

}