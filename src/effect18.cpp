#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "main.h"
#include "messages.h"
#include "stars.h"

static int frame_count = 0; /* frames drawn */

static string effect18_name = "Stars only";

/*static GLdouble width=0.0, height=0.0;*/
static GLfloat rot[3] = {0.0, 0.0, 0.0};
/* initial rotation speeds */
static GLfloat w[3] = {0.1, 0.1, 0.1};
#define MY_Z0 8.0
static GLfloat my_z = MY_Z0;

static GLfloat fontcolor[3] = {1.0, 1.0, 0.0};

static void effect18_display_cb(void)
{
	int i;
	char s[256];

	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, my_z,        /* eye */
		  0.0, 0.0, my_z - 1.0,  /* ctr */
		  0.0, 1.0, 0.0);        /* up */
	glRotatef(rot[0], 1.0, 0.0, 0.0);
	glRotatef(rot[1], 0.0, 1.0, 0.0);
	glRotatef(rot[2], 0.0, 0.0, 1.0);
	for (i = 0; i < 3; i++) {
		rot[i] += w[i];
		if (rot[i] >= 360.0)
			rot[i] -= 360.0;
		if (rot[i] < 0.0)
			rot[i] += 360.0;
	}

	stars_draw();

	glRotatef(90.0, 1.0, 0.0, 0.0);

	glRotatef(90.0, 0.0, 1.0, 0.0);

	glDisable(GL_LIGHTING);

	/* handle text to user and pause */
	if (messages_on() || pause_is_requested()) {
		sprintf(s, "w0: %.1f w1: %.1f w2: %.1f", w[0], w[1], w[2]);
		messages_print(s, frame_count, fontcolor);
	}

	/* activate pause if requested */
	if (pause_is_requested())
		pause_activate();

	frame_count++;
}

static void effect18_reshape_cb(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
}

static void effect18_keyboard_cb(SDL_KeyboardEvent key)
{
	switch (key.keysym.sym) {
	case SDLK_s: /* status */
	case SDLK_m:
		messages_toggle();
		break;
	case SDLK_p:
		pause_request();
		break;

	case SDLK_ESCAPE:
		return_to_calendar();
		break;
	default:
		break;
	}

}

int Effect18::init()
{
	glBlendFunc(GL_ONE, GL_ONE);
	frame_count = 0; /* zero frame count */

	return 0;
}

void Effect18::cleanup()
{

}

int effect18_register(struct effect *ep)
{
	ep->e_display  = effect18_display_cb;
	ep->e_reshape  = effect18_reshape_cb;
	ep->e_keyboard = effect18_keyboard_cb;

	ep->e_name     = effect18_name;
	return 0;
}



