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

		S = SDL_SCANCODE_S,
		W = SDL_SCANCODE_W,
		X = SDL_SCANCODE_X,

		Z = SDL_SCANCODE_Z,

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