#ifndef UHERO_INPUT_H__
#define UHERO_INPUT_H__ 1

#include "common.hpp"
#include <SDL2/SDL_scancode.h>

namespace uhero
{
	enum KeyCode : u16
	{
		Escape = SDL_SCANCODE_ESCAPE,

		A = SDL_SCANCODE_A,
		B = SDL_SCANCODE_B,
		C = SDL_SCANCODE_C,
		D = SDL_SCANCODE_D,
		E = SDL_SCANCODE_E,
		F = SDL_SCANCODE_F,
		G = SDL_SCANCODE_G,
		H = SDL_SCANCODE_H,
		I = SDL_SCANCODE_I,
		J = SDL_SCANCODE_J,
		K = SDL_SCANCODE_K,
		L = SDL_SCANCODE_L,
		M = SDL_SCANCODE_M,
		N = SDL_SCANCODE_N,
		O = SDL_SCANCODE_O,
		P = SDL_SCANCODE_P,
		Q = SDL_SCANCODE_Q,
		R = SDL_SCANCODE_R,
		S = SDL_SCANCODE_S,
		T = SDL_SCANCODE_T,
		U = SDL_SCANCODE_U,
		V = SDL_SCANCODE_V,
		W = SDL_SCANCODE_W,
		X = SDL_SCANCODE_X,
		Y = SDL_SCANCODE_Y,
		Z = SDL_SCANCODE_Z,

		D1 = SDL_SCANCODE_1,
		D2 = SDL_SCANCODE_2,
		D3 = SDL_SCANCODE_3,
		D4 = SDL_SCANCODE_4,
		D5 = SDL_SCANCODE_5,
		D6 = SDL_SCANCODE_6,
		D7 = SDL_SCANCODE_7,
		D8 = SDL_SCANCODE_8,
		D9 = SDL_SCANCODE_9,
		D0 = SDL_SCANCODE_0,

		LControl = SDL_SCANCODE_LCTRL,
		RControl = SDL_SCANCODE_RCTRL,
		LShift = SDL_SCANCODE_LSHIFT,
		RShift = SDL_SCANCODE_RSHIFT,
		LAlt = SDL_SCANCODE_LALT,
		RAlt = SDL_SCANCODE_RALT,
		Tab = SDL_SCANCODE_TAB,
		CapsLock = SDL_SCANCODE_CAPSLOCK,

		Space = SDL_SCANCODE_SPACE,
		Backspace = SDL_SCANCODE_BACKSPACE,
		Insert = SDL_SCANCODE_INSERT,
		Delete = SDL_SCANCODE_DELETE,
		Return = SDL_SCANCODE_RETURN,
		Return2 = SDL_SCANCODE_RETURN2,
		Comma = SDL_SCANCODE_COMMA,
		Period = SDL_SCANCODE_PERIOD,
		Slash = SDL_SCANCODE_SLASH,

		ArrowUp = SDL_SCANCODE_UP,
		ArrowDown = SDL_SCANCODE_DOWN,
		ArrowLeft = SDL_SCANCODE_LEFT,
		ArrowRight = SDL_SCANCODE_RIGHT,

		MaxKey = 512,
	};

	enum MouseKey : u8
	{
		Left,
		Middle,
		Right,

		MaxMKey = 3,
	};

	enum KeyState : u8
	{
		Up = 0,
		Down = 1 << 0,
		Pressed = 1 << 1,
		Released = 1 << 2,
		Repeat = 1 << 3,
	};

	struct MouseState
	{
		u8 buttons[MouseKey::MaxMKey];
		i32 x;
		i32 y;

		void reset()
		{
			x = 0;
			y = 0;
			for (u8 i = 0; i < MouseKey::MaxMKey; i++)
				buttons[i] = KeyState::Up;
		}
	};

	struct Input
	{
		u8 keys[KeyCode::MaxKey];
		MouseState mouse;

		void reset()
		{
			mouse.reset();
			for (u16 i = 0; i < KeyCode::MaxKey; i++)
				keys[i] = KeyState::Up;
		}

		void set_keystate(u32 key, KeyState state)
		{
			u16 index = static_cast<u16>(key);
			keys[index] |= static_cast<u8>(state);
		}

		bool is_key_down(KeyCode key) const
		{
			return keys[key] & KeyState::Down;
		}

		bool is_key_pressed(KeyCode key) const
		{
			return keys[key] & KeyState::Pressed;
		}

		bool is_key_released(KeyCode key) const
		{
			return keys[key] & KeyState::Released;
		}

		// mouse 
		bool is_mbutton_down(MouseKey key) const
		{
			return mouse.buttons[key] & KeyState::Down;
		}

		bool is_mbutton_pressed(MouseKey key) const
		{
			return mouse.buttons[key] & KeyState::Pressed;
		}

		bool is_mbutton_released(MouseKey key) const
		{
			return mouse.buttons[key] & KeyState::Released;
		}
	};
}

#endif