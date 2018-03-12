#include "main.h"

#include <stdlib.h>
#include <string.h>

/*
 * This pausing can be used by effects to freeze their screens.
 * It should take care of other main callbacks also, but who
 * cares.
 */

static int pausing = 0;
static int reshape_width = -1;
static int reshape_height = -1;

static void pause_display_cb(void);
static void pause_keyboard_cb(SDL_KeyboardEvent key);
static void pause_reshape_cb(int width, int height);


void pause_request(void)
{
	cout << "pause_request\n";
	pausing = 1;
}

void pause_activate(void)
{
	struct effect e;

	cout << "pausing...\n";

	reshape_width = -1;
	reshape_height = -1;

	memset(&e, 0, sizeof(struct effect));
	e.e_display = pause_display_cb;
	e.e_reshape = pause_reshape_cb;
	e.e_keyboard = pause_keyboard_cb;

	main_set_callbacks(&e, -1, -1);
}

int pause_is_requested(void)
{
	return pausing;
}



static void pause_display_cb(void)
{
	return;
}

static void pause_keyboard_cb(SDL_KeyboardEvent key)
{
	cout << "pause keyboard cb\n";

	switch (key.keysym.sym) {
		/* quit pausing */
	case SDLK_SPACE:
	case SDLK_p:
	case SDLK_ESCAPE:
		pausing = 0;
		cout << "current effect is " << current_effect->e_name << "\n";
		main_set_callbacks(current_effect, 
					reshape_width, reshape_height);
		break;
	default:
		break;
	}
}

static void pause_reshape_cb(int width, int height)
{
	reshape_width = width;
	reshape_height = height;

	cout << "reshape called during pause ...\n";
}

