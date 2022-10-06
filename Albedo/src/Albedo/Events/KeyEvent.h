#pragma once

#include "Event.h"

namespace Albedo {
	class Albedo_API KeyEvent : public Event {
	public:
		inline int GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)
	protected:
		Event(int keycode)
			:m_KeyCode(keycode) {}
	private:
		int m_KeyCode;
	};

	class Albedo_API KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(int keycode, int repeatcount)
			:m_Keycode(keycode), m_RepeatCount(repeatcount) {}

		inline int GetRepeatCoun() const { return m_RepeatCount; }

		std::string ToString const override{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}
			EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};

	class Albedo_API KeyReleasedEvent : public KeyEvent {
	public:
		KeyPressedEvent(int keycode)
			:m_KeyCode(keycode) {}

		std::string ToString() {
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}