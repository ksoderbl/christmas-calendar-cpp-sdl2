#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "main.h"
#include "stars.h"

static string effect16_name = "\"Brownian\" motion";

/* state variables */


static int frame_count = 0;      /* number of frames drawn */

static GLfloat rot[3] = {0.0, 0.0, 0.0};
static GLfloat w[3] = {0.1, 0.1, 0.1};
#define MY_Z0 15.0
static GLfloat my_z = MY_Z0;

#define XMAX 10.0

static GLfloat fontcolor[3] = {0.0, 0.4, 0.9};




#define NUMPOINTS 20000

GLfloat p    [NUMPOINTS][3];
GLfloat c    [NUMPOINTS][3];

GLfloat point[3]      = {0.0, 0.0, 0.0};
GLfloat pointcolor[3] = {1.0, 1.0, 1.0};

int numpoints = 0;


static GLfloat RNDINCR(void)
{
	if (rand()&1)
		return (((GLfloat)(rand()))/RAND_MAX)/5;
	else
		return -(((GLfloat)(rand()))/RAND_MAX)/5;
}


static void effect16_reset(void)
{
	int i;
	
	for (i = 0; i < 3; i++) {
		rot[i] = 0.0;
		w[i] = 0.0;
	}
	my_z = MY_Z0;
}






static void effect16_display_cb(void)
{
	int i;
	char s[256];

	/* initshit */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


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

	/* add a point */
		
	p[numpoints][0] = point[0] + RNDINCR();
	p[numpoints][1] = point[1] + RNDINCR();
	p[numpoints][2] = point[2] + RNDINCR();

	c[numpoints][0] = pointcolor[0] + RNDINCR()*3;
	c[numpoints][1] = pointcolor[1] + RNDINCR()*3;
	c[numpoints][2] = pointcolor[2] + RNDINCR()*3;

	for (i = 0; i < 3; i++) {
		/* 0 =< color components <= 1 */ 
		if (c[numpoints][i] > 1.0)
			c[numpoints][i] = 1.0;
		if (c[numpoints][i] < 0.0)
			c[numpoints][i] = 0.0;
		/* -XMAX =< spatial coordinates <= XMAX */ 
		if (p[numpoints][i] > XMAX)
			p[numpoints][i] = XMAX;
		if (p[numpoints][i] < -XMAX)
			p[numpoints][i] = -XMAX;
		
		point[i] = p[numpoints][i];
		pointcolor[i] = c[numpoints][i];
	}

	if (numpoints < NUMPOINTS)
		numpoints++;



	/*glPushMatrix();*/
	glBegin(/*GL_POINTS*/ GL_LINE_STRIP);
	for (i = 0; i < numpoints; i++) {
		glColor3fv(c[i]);
		glVertex3fv(p[i]);
	}
	glEnd();
	/*glPopMatrix();*/


	stars_draw();  /* stars.h */

	if (messages_on() || pause_is_requested()) {
		sprintf(s, "number of vertices: %d", numpoints);
		messages_print(s, frame_count, fontcolor);
	}

	/* activate pause if requested */
	if (pause_is_requested())
		pause_activate();

	frame_count++;
}

static void effect16_reshape_cb(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
}







static void effect16_keyboard_cb(SDL_KeyboardEvent key)
{
	switch (key.keysym.sym) {
	case SDLK_s: /* status */
	case SDLK_m:
		messages_toggle();
		break;
	case SDLK_p:
		pause_request();
		break;
	case SDLK_r:
		effect16_reset();
		break;
	case SDLK_x:
		w[2] += 0.1;
		break;
	case SDLK_z:
		w[2] -= 0.1;
		break;

	case SDLK_ESCAPE:
		return_to_calendar();
		break;
	case SDLK_PAGEUP:
		my_z -= 2.0;
		if (my_z <= 0.0)
			my_z = 0.0;
		break;
	case SDLK_PAGEDOWN:
		my_z += 2.0;
		if (my_z >= STARS_MINDIST)
			my_z = STARS_MINDIST;
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
	default:
		break;
	}

}

int Effect16::init()
{
#if 0
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
#endif

	frame_count = 0;

	return 0;
}

void Effect16::cleanup()
{

}

int effect16_register(struct effect *ep)
{
	ep->e_display  = effect16_display_cb;
	ep->e_reshape  = effect16_reshape_cb;
	ep->e_keyboard = effect16_keyboard_cb;

	ep->e_name     = effect16_name;
	return 0;
}

