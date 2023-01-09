#pragma once

namespace Albedo
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

// From glfw3.h
#define Albedo_KEY_SPACE           ::Albedo::Key::Space
#define Albedo_KEY_APOSTROPHE      ::Albedo::Key::Apostrophe    /* ' */
#define Albedo_KEY_COMMA           ::Albedo::Key::Comma         /* , */
#define Albedo_KEY_MINUS           ::Albedo::Key::Minus         /* - */
#define Albedo_KEY_PERIOD          ::Albedo::Key::Period        /* . */
#define Albedo_KEY_SLASH           ::Albedo::Key::Slash         /* / */
#define Albedo_KEY_0               ::Albedo::Key::D0
#define Albedo_KEY_1               ::Albedo::Key::D1
#define Albedo_KEY_2               ::Albedo::Key::D2
#define Albedo_KEY_3               ::Albedo::Key::D3
#define Albedo_KEY_4               ::Albedo::Key::D4
#define Albedo_KEY_5               ::Albedo::Key::D5
#define Albedo_KEY_6               ::Albedo::Key::D6
#define Albedo_KEY_7               ::Albedo::Key::D7
#define Albedo_KEY_8               ::Albedo::Key::D8
#define Albedo_KEY_9               ::Albedo::Key::D9
#define Albedo_KEY_SEMICOLON       ::Albedo::Key::Semicolon     /* ; */
#define Albedo_KEY_EQUAL           ::Albedo::Key::Equal         /* = */
#define Albedo_KEY_A               ::Albedo::Key::A
#define Albedo_KEY_B               ::Albedo::Key::B
#define Albedo_KEY_C               ::Albedo::Key::C
#define Albedo_KEY_D               ::Albedo::Key::D
#define Albedo_KEY_E               ::Albedo::Key::E
#define Albedo_KEY_F               ::Albedo::Key::F
#define Albedo_KEY_G               ::Albedo::Key::G
#define Albedo_KEY_H               ::Albedo::Key::H
#define Albedo_KEY_I               ::Albedo::Key::I
#define Albedo_KEY_J               ::Albedo::Key::J
#define Albedo_KEY_K               ::Albedo::Key::K
#define Albedo_KEY_L               ::Albedo::Key::L
#define Albedo_KEY_M               ::Albedo::Key::M
#define Albedo_KEY_N               ::Albedo::Key::N
#define Albedo_KEY_O               ::Albedo::Key::O
#define Albedo_KEY_P               ::Albedo::Key::P
#define Albedo_KEY_Q               ::Albedo::Key::Q
#define Albedo_KEY_R               ::Albedo::Key::R
#define Albedo_KEY_S               ::Albedo::Key::S
#define Albedo_KEY_T               ::Albedo::Key::T
#define Albedo_KEY_U               ::Albedo::Key::U
#define Albedo_KEY_V               ::Albedo::Key::V
#define Albedo_KEY_W               ::Albedo::Key::W
#define Albedo_KEY_X               ::Albedo::Key::X
#define Albedo_KEY_Y               ::Albedo::Key::Y
#define Albedo_KEY_Z               ::Albedo::Key::Z
#define Albedo_KEY_LEFT_BRACKET    ::Albedo::Key::LeftBracket   /* [ */
#define Albedo_KEY_BACKSLASH       ::Albedo::Key::Backslash     /* \ */
#define Albedo_KEY_RIGHT_BRACKET   ::Albedo::Key::RightBracket  /* ] */
#define Albedo_KEY_GRAVE_ACCENT    ::Albedo::Key::GraveAccent   /* ` */
#define Albedo_KEY_WORLD_1         ::Albedo::Key::World1        /* non-US #1 */
#define Albedo_KEY_WORLD_2         ::Albedo::Key::World2        /* non-US #2 */

/* Function keys */
#define Albedo_KEY_ESCAPE          ::Albedo::Key::Escape
#define Albedo_KEY_ENTER           ::Albedo::Key::Enter
#define Albedo_KEY_TAB             ::Albedo::Key::Tab
#define Albedo_KEY_BACKSPACE       ::Albedo::Key::Backspace
#define Albedo_KEY_INSERT          ::Albedo::Key::Insert
#define Albedo_KEY_DELETE          ::Albedo::Key::Delete
#define Albedo_KEY_RIGHT           ::Albedo::Key::Right
#define Albedo_KEY_LEFT            ::Albedo::Key::Left
#define Albedo_KEY_DOWN            ::Albedo::Key::Down
#define Albedo_KEY_UP              ::Albedo::Key::Up
#define Albedo_KEY_PAGE_UP         ::Albedo::Key::PageUp
#define Albedo_KEY_PAGE_DOWN       ::Albedo::Key::PageDown
#define Albedo_KEY_HOME            ::Albedo::Key::Home
#define Albedo_KEY_END             ::Albedo::Key::End
#define Albedo_KEY_CAPS_LOCK       ::Albedo::Key::CapsLock
#define Albedo_KEY_SCROLL_LOCK     ::Albedo::Key::ScrollLock
#define Albedo_KEY_NUM_LOCK        ::Albedo::Key::NumLock
#define Albedo_KEY_PRINT_SCREEN    ::Albedo::Key::PrintScreen
#define Albedo_KEY_PAUSE           ::Albedo::Key::Pause
#define Albedo_KEY_F1              ::Albedo::Key::F1
#define Albedo_KEY_F2              ::Albedo::Key::F2
#define Albedo_KEY_F3              ::Albedo::Key::F3
#define Albedo_KEY_F4              ::Albedo::Key::F4
#define Albedo_KEY_F5              ::Albedo::Key::F5
#define Albedo_KEY_F6              ::Albedo::Key::F6
#define Albedo_KEY_F7              ::Albedo::Key::F7
#define Albedo_KEY_F8              ::Albedo::Key::F8
#define Albedo_KEY_F9              ::Albedo::Key::F9
#define Albedo_KEY_F10             ::Albedo::Key::F10
#define Albedo_KEY_F11             ::Albedo::Key::F11
#define Albedo_KEY_F12             ::Albedo::Key::F12
#define Albedo_KEY_F13             ::Albedo::Key::F13
#define Albedo_KEY_F14             ::Albedo::Key::F14
#define Albedo_KEY_F15             ::Albedo::Key::F15
#define Albedo_KEY_F16             ::Albedo::Key::F16
#define Albedo_KEY_F17             ::Albedo::Key::F17
#define Albedo_KEY_F18             ::Albedo::Key::F18
#define Albedo_KEY_F19             ::Albedo::Key::F19
#define Albedo_KEY_F20             ::Albedo::Key::F20
#define Albedo_KEY_F21             ::Albedo::Key::F21
#define Albedo_KEY_F22             ::Albedo::Key::F22
#define Albedo_KEY_F23             ::Albedo::Key::F23
#define Albedo_KEY_F24             ::Albedo::Key::F24
#define Albedo_KEY_F25             ::Albedo::Key::F25

/* Keypad */
#define Albedo_KEY_KP_0            ::Albedo::Key::KP0
#define Albedo_KEY_KP_1            ::Albedo::Key::KP1
#define Albedo_KEY_KP_2            ::Albedo::Key::KP2
#define Albedo_KEY_KP_3            ::Albedo::Key::KP3
#define Albedo_KEY_KP_4            ::Albedo::Key::KP4
#define Albedo_KEY_KP_5            ::Albedo::Key::KP5
#define Albedo_KEY_KP_6            ::Albedo::Key::KP6
#define Albedo_KEY_KP_7            ::Albedo::Key::KP7
#define Albedo_KEY_KP_8            ::Albedo::Key::KP8
#define Albedo_KEY_KP_9            ::Albedo::Key::KP9
#define Albedo_KEY_KP_DECIMAL      ::Albedo::Key::KPDecimal
#define Albedo_KEY_KP_DIVIDE       ::Albedo::Key::KPDivide
#define Albedo_KEY_KP_MULTIPLY     ::Albedo::Key::KPMultiply
#define Albedo_KEY_KP_SUBTRACT     ::Albedo::Key::KPSubtract
#define Albedo_KEY_KP_ADD          ::Albedo::Key::KPAdd
#define Albedo_KEY_KP_ENTER        ::Albedo::Key::KPEnter
#define Albedo_KEY_KP_EQUAL        ::Albedo::Key::KPEqual

#define Albedo_KEY_LEFT_SHIFT      ::Albedo::Key::LeftShift
#define Albedo_KEY_LEFT_CONTROL    ::Albedo::Key::LeftControl
#define Albedo_KEY_LEFT_ALT        ::Albedo::Key::LeftAlt
#define Albedo_KEY_LEFT_SUPER      ::Albedo::Key::LeftSuper
#define Albedo_KEY_RIGHT_SHIFT     ::Albedo::Key::RightShift
#define Albedo_KEY_RIGHT_CONTROL   ::Albedo::Key::RightControl
#define Albedo_KEY_RIGHT_ALT       ::Albedo::Key::RightAlt
#define Albedo_KEY_RIGHT_SUPER     ::Albedo::Key::RightSuper
#define Albedo_KEY_MENU            ::Albedo::Key::Menu