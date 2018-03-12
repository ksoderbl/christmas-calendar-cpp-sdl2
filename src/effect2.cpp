/* pulsar.c V2.0 - kps - 2001 */

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

#define N1 512
#define R1 3.5
#define A1 10
#define B1 10

#define N2 512
#define R2 5
#define A2 20
#define B2 20

#define N3 512
#define R3 4
#define A3 30
#define B3 30





static int blend = 1;
static int frame_count = 0; /* frames drawn */

static string effect2_name = "\"Pulsar\"";




/*static GLdouble width=0.0, height=0.0;*/
static GLfloat rot[3] = {0.0, 0.0, 0.0};
/* initial rotation speeds */
static GLfloat w[3] = {0.1, 0.1, 0.1};
#define MY_Z0 8.0
static GLfloat my_z = MY_Z0;

static GLfloat fontcolor[3] = {1.0, 1.0, 0.0};

static void reset(void);



static void effect2_display_cb(void)
{
	int i;
	double angle = 0.0;
	double ax=0.0, bx=0.0;
	double sinax = 0.0, cosbx = 0.0;
	double cosax = 0.0, sinbx = 0.0;
	double r = 0.0;
	GLfloat x=0.0, y=0.0;
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

	if (blend)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	glBegin(GL_LINE_LOOP);
	for (i = 0; i < N1; i++) {
		angle = (double)2*M_PI*i/N1;
		ax = A1 * angle;
		bx = (double)(frame_count * angle / B1);
		sinax = sin(ax);
		cosbx = cos(bx);
		
		r = sinax * cosbx;
		x = r * cos(angle);
		y = r * sin(angle);
		glColor4f((GLfloat)sinax, (GLfloat)cosbx, (GLfloat)cosax, 0.9);
		glVertex2f(R1 * x, R1 *y);
	}
	glEnd();



	glRotatef(90.0, 1.0, 0.0, 0.0);

	glBegin(GL_LINE_LOOP);
	for (i = 0; i < N2; i++) {
		angle = (double)2*M_PI*i/N2;
		ax = A2 * angle;
		bx = (double)(frame_count * angle / B2);
		sinax = sin(ax);
		cosbx = cos(bx);
		sinbx = sin(bx);
		cosax = cos(ax);
		
		r = sinax * cosbx * sinbx;
		x = r * cos(angle);
		y = r * sin(angle);
		glColor4f((GLfloat)cosbx, (GLfloat)cosax, (GLfloat)sinax, 0.8);
		glVertex2f(R2 * x, R2 * y);
	}
	glEnd();


	glRotatef(90.0, 0.0, 1.0, 0.0);
	glBegin(GL_LINE_LOOP);
	for (i = 0; i < N3; i++) {
		angle = (double)2*M_PI*i/N3;
		ax = A3 * angle;
		bx = (double)(frame_count * angle / B3);
		sinax = sin(ax);
		cosbx = cos(bx);
		sinbx = sin(bx);
		cosax = cos(ax);
		
		r = sinax * cosax * cosbx;
		x = r * cos(angle);
		y = r * sin(angle);
		glColor4f((GLfloat)cosax, (GLfloat)sinax, (GLfloat)cosbx, 0.7);
		glVertex2f(R3 * x, R3 * y);
	}
	glEnd();

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

static void effect2_reshape_cb(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
}

static void effect2_keyboard_cb(SDL_KeyboardEvent key)
{
	switch (key.keysym.sym) {
	case SDLK_b:
		blend = !blend;
		break;
	case SDLK_s: /* status */
	case SDLK_m:
		messages_toggle();
		break;
	case SDLK_p:
		pause_request();
		break;
	case SDLK_r:
		reset();
		break;
	case SDLK_x:
		w[2] += 0.1;
		break;
	case SDLK_z:
		w[2] -= 0.1;
		break;
	case SDLK_PAGEUP:
		my_z -= 0.5;
		if (my_z <= 1.0)
			my_z = 1.0;
		break;
	case SDLK_PAGEDOWN:
		my_z += 0.5;
		break;
	case SDLK_LEFT:
		w[1] -= 0.1;
		break;
	case SDLK_RIGHT:
		w[1] += 0.1;
		break;
	case SDLK_UP:
		w[0] -= 0.1;
		break;
	case SDLK_DOWN:
		w[0] += 0.1;
		break;
	case SDLK_ESCAPE:
		return_to_calendar();
		break;
	default:
		break;
	}
}


static void reset(void)
{
	int i;
	
	for (i = 0; i < 3; i++) {
		rot[i] = 0.0;
		w[i] = 0.0;
	}
	my_z = MY_Z0;
}

int Effect2::init()
{
#if 0
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
#endif
	glBlendFunc(GL_ONE, GL_ONE);
	frame_count = 0; /* zero frame count */

	/*reset();*/
	return 0;
}

void Effect2::cleanup()
{
	return;
}

int effect2_register(struct effect *ep)
{
	ep->e_display  = effect2_display_cb;
	ep->e_reshape  = effect2_reshape_cb;
        ep->e_keyboard = effect2_keyboard_cb;
        /*ep->e_mouse    = effect2_mouse_cb;*/

	ep->e_name     = effect2_name;
	return 0;
}



