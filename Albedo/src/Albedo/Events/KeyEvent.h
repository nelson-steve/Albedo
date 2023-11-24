#pragma once

#include "Event.h"
#include "Albedo/Core/KeyCodes.h"

namespace Albedo {

	class Albedo_API KeyEvent : public Event {
	public:
		KeyEvent() {}
		inline KeyCode GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)
	protected:
		KeyEvent(KeyCode keycode)
			:m_KeyCode(keycode) {}
	protected:
		KeyCode m_KeyCode;
	};

	class Albedo_API KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent() {}
		KeyPressedEvent(int keycode, int repeatcount)
			:KeyEvent(static_cast<KeyCode>(keycode)), m_RepeatCount(repeatcount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override{
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
		KeyReleasedEvent(int keycode)
			:KeyEvent(static_cast<KeyCode>(keycode)) {}

		std::string ToString() {
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

}