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

string Effect18::getName()
{
	return "Stars only";
}

/*static GLdouble width=0.0, height=0.0;*/
static GLfloat rot[3] = {0.0, 0.0, 0.0};
/* initial rotation speeds */
static GLfloat w[3] = {0.1, 0.1, 0.1};
#define MY_Z0 8.0
static GLfloat my_z = MY_Z0;

static GLfloat fontcolor[3] = {1.0, 1.0, 0.0};

void Effect18::drawFrame()
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
	if (messages_on()) {
		sprintf(s, "w0: %.1f w1: %.1f w2: %.1f", w[0], w[1], w[2]);
		messages_print(s, frame_count, fontcolor);
	}

	frame_count++;
}

void Effect18::resize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
}

void Effect18::mouseButtonDownEvent(SDL_MouseButtonEvent button)
{
}

void Effect18::mouseMotionEvent(SDL_MouseMotionEvent motion)
{
}

void Effect18::keyboardEvent(SDL_KeyboardEvent key)
{
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

void Effect18::reset()
{
}
