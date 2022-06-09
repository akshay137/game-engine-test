#ifndef UHERO_INPUT_H__
#define UHERO_INPUT_H__ 1

#include "core.h"

enum keystate_e
{
	UHKEY_UP = 0,
	UHKEY_DOWN = 1,
	UHKEY_PRESSED = 2 | UHKEY_DOWN,
	UHKEY_RELEASED = 4,
};
typedef enum keystate_e keystate_t;

typedef uint32_t keycode_t;

#define UH_MAX_KEYS 512

struct input_s
{
	int32_t mouse_x;
	int32_t mouse_y;
	keystate_t keys[UH_MAX_KEYS];
};

API_INLINE bool ip_keyDown(const input_t* ip, keycode_t key)
{
	return UHKEY_DOWN & ip->keys[key];
}

API_INLINE bool ip_keyUp(const input_t* ip, keycode_t key)
{
	return UHKEY_UP == ip->keys[key];
}

API_INLINE bool ip_keyPressed(const input_t* ip, keycode_t key)
{
	return UHKEY_PRESSED == ip->keys[key];
}

API_INLINE bool ip_keyReleased(const input_t* ip, keycode_t key)
{
	return UHKEY_RELEASED == ip->keys[key];
}

#endif