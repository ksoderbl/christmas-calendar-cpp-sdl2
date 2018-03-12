#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "main.h"
#include "stars.h"
#include "util.h"

static string effect8_name = "Gouraud shaded Icosahedron";

static GLdouble x_width;
static GLdouble x_height;
static GLdouble x_fovy = 40.0;

/* state variables */

static int frame_count = 0;      /* number of frames drawn */


static GLfloat rot[3] = {0.0, 0.0, 0.0};
/* initial rotation speeds of cubes */
static GLfloat w[3] = {0.1, 0.2, 0.3};
#define MY_Z0 6.0
static GLfloat my_z = MY_Z0;

static GLfloat srot[3] = {0.0, 0.0, 0.0};

static GLfloat fontcolor[3] = {0.9, 0.8, 0.3};


/* redbook icosahedron data */
#define X .525731112119133606
#define Z .850650808352039932

static GLfloat vdata[12][3] = {
	{-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
	{0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
	{Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};

static GLuint tindices[20][3] = {
	{0, 4, 1}, {0, 9, 4}, {9, 5, 4}, {4, 5, 8}, {4, 8, 1},
	{8, 10, 1},{8, 3, 10},{5, 3, 8}, {5, 2, 3}, {2, 7, 3},
	{7, 10, 3},{7, 6, 10},{7, 11, 6},{11, 0, 6},{0, 1, 6},
	{6, 1, 10},{9, 0, 11},{9, 11, 2},{9, 2, 5}, {7, 2, 11},
};
static GLfloat vcolors[12][3];

static GLfloat colors[8][3] = {
	{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0},
	{0.0, 1.0, 0.0}, {1.0, 1.0, 0.0},
	{0.0, 0.0, 1.0}, {1.0, 0.0, 1.0},
	{0.0, 1.0, 1.0}, {1.0, 1.0, 1.0}
}; 


static void randomize_icosahedron_colors(void)
{
	int i;

	if (rand()&1) {
		for (i = 0; i < 12; i++)
			memcpy (&vcolors[i],
				&colors[ rand()&7 ],
				3 * sizeof(GLfloat));
	} else {
		for (i = 0; i < 12; i++) {
			vcolors[i][0] = (GLfloat)randdouble();
			vcolors[i][1] = (GLfloat)randdouble();
			vcolors[i][2] = (GLfloat)randdouble();
		}
	}
}


static void effect8_reset(void)
{
	int i;
	
	for (i = 0; i < 3; i++) {
		rot[i] = 0.0;
		w[i] = 0.0;
	}
	my_z = MY_Z0;
}








static void effect8_display_cb(void)
{
	int i;
	char s[256];

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(x_fovy, x_width/x_height, 0.1, 1000.0);

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
	
	glPushMatrix();
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

	
	glBegin(GL_TRIANGLES);
	for (i = 0; i < 20; i++) {
		/* color */
		glColor3fv  ( &vcolors [ tindices [i][0] ] [0] );
		glVertex3fv ( &vdata   [ tindices [i][0] ] [0] );
		glColor3fv  ( &vcolors [ tindices [i][1] ] [0] );
		glVertex3fv ( &vdata   [ tindices [i][1] ] [0] );
		glColor3fv  ( &vcolors [ tindices [i][2] ] [0] );
		glVertex3fv ( &vdata   [ tindices [i][2] ] [0] );
	}
	glEnd();

	/* ellipses */
#define EDGES 50
#define RAD 1.4

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
	for (i = 0; i < EDGES; i++) {
		glVertex2f(RAD * cos((2*M_PI*i)/EDGES),
			   RAD * sin((2*M_PI*i)/EDGES));
	}
	glEnd();
	glBegin(GL_LINE_LOOP);
	for (i = 0; i < EDGES; i++) {
		glVertex2f(RAD*1.1 * cos((2*M_PI*i)/EDGES),
			   RAD*1.3 * sin((2*M_PI*i)/EDGES));
	}
	glEnd();


	glPopMatrix();




	glTranslatef(0.0, 0.0, my_z);
	glRotatef(srot[0], 1.0, 0.0, 0.0);
	glRotatef(srot[1], 0.0, 1.0, 0.0);
	glRotatef(srot[2], 0.0, 0.0, 1.0);
	srot[0] += 0.1;
	srot[1] += 0.2;
	srot[2] += 0.1;

	stars_draw();

	if (messages_on() || pause_is_requested()) {
		sprintf(s, "field of view is %.2f degrees"
			" (press 'z' or 'x' to adjust)", 
			x_fovy);
		messages_print(s, frame_count, fontcolor);
	}

	/* activate pause if requested */
	if (pause_is_requested())
		pause_activate();

	frame_count++;
}

static void effect8_reshape_cb(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	x_width = (GLdouble)w;
	x_height = (GLdouble)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(x_fovy, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
}

static void effect8_keyboard_cb(SDL_KeyboardEvent key)
{
	switch (key.keysym.sym) {
	case SDLK_SPACE:
		randomize_icosahedron_colors();
		break;
	case SDLK_m:
		messages_toggle();
		break;
	case SDLK_p:
		pause_request();
		break;
	case SDLK_r:
		effect8_reset();
		break;
	case SDLK_x:
		x_fovy += 1.0;
		/*w[2] += 0.1;*/
		break;
	case SDLK_z:
		x_fovy -= 1.0;
		/*w[2] -= 0.1;*/
		break;
	case SDLK_PAGEUP:
		my_z -= 1.0;
		if (my_z <= 0.0)
			my_z = 0.0;
		break;
	case SDLK_PAGEDOWN:
		my_z += 1.0;
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

int Effect8::init()
{
	randomize_icosahedron_colors();

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);


	frame_count = 0;

	return 0;
}

void Effect8::cleanup()
{

}

int effect8_register(struct effect *ep)
{
	ep->e_display  = effect8_display_cb;
	ep->e_reshape  = effect8_reshape_cb;
	ep->e_keyboard = effect8_keyboard_cb;

	ep->e_name     = effect8_name;
	return 0;
}


