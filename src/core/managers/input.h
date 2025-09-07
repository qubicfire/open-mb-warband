#ifndef _INPUT_H
#define _INPUT_H
#include <glm/ext.hpp>

#include "core/mb.h"
#include "core/mb_type_traits.h"
#include "core/platform/window.h"

enum class KeyCode : int16_t
{
	None = 0,
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
	Menu = 348,
	Count
};

enum class MouseCode : int8_t
{
	Left,
	Right,
	Middle,
	Mouse1,
	Mouse2,
	Mouse3,
	Mouse4,
	Mouse5,
	Mouse6,
	Count
};

enum class InputAxis : int8_t
{
	Horizontal,
	Vertical,
};

class Input final
{
	friend class Engine;
public:
	static void initialize(const Unique<mbcore::Window>& window) noexcept;

	static void set_cursor_visible(bool state);

	static void set_mouse_origin(const double x, const double y);
	static bool get_mouse_button(const MouseCode code) noexcept;
	static bool get_mouse_button_down(const MouseCode code) noexcept;
	static bool get_mouse_button_up(const MouseCode code) noexcept;
	static bool has_mouse_origin_changed() noexcept;
	static glm::vec2& get_mouse_origin() noexcept;

	// Return true if key is being held
	static bool get_key(const KeyCode code) noexcept;
	// Return true if method and key was called and pressed on the same frame
	static bool get_key_down(const KeyCode code) noexcept;
	static bool get_key_up(const KeyCode code) noexcept;
	
	// Return last pressed key or mouse button.
	// Method is using by UI. I have no idea where you could use this
	// TODO: KeyCode -> uint32_t/uint16_t. METHOD DOESN'T RETURN MOUSE BUTTON
	static KeyCode get_any_pressed_key() noexcept;

	// TODO: Implement
	static float get_axis(const InputAxis axis);
	// TODO: Implement
	static glm::vec3 get_axis_dimension(const InputAxis axis);
private:
	// Events triggered by different API's (GLFW, Vulkan, etc.) 

	static void set_mouse_button_pressed(const int code);
	static void set_mouse_button_released(const int code);
	static void set_key_pressed(const int code);
	static void set_key_released(const int code);
private:
	static inline KeyCode m_last_key {};
	static inline glm::vec2 m_mouse_origin {};
	static inline uint32_t m_mouse_origin_frame {};

	// Structs will be at least 8 bytes, so fuck it
	static inline bool m_mouse_keys[static_cast<size_t>(MouseCode::Count)];
	static inline uint32_t m_mouse_frames[static_cast<size_t>(MouseCode::Count)];
	static inline bool m_keys[static_cast<size_t>(KeyCode::Count)];
	static inline uint32_t m_frames[static_cast<size_t>(KeyCode::Count)];

	static inline mbcore::Window* m_window;
};

#endif // !_INPUT_H
