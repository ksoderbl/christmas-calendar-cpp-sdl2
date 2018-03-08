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

static string effect4_name = "Ball";

#define SUBDIVISION_DEPTH 4


/* state variables */

static int frame_count = 0;      /* number of frames drawn */


static GLuint list;

static GLfloat rot[3] = {0.0, 0.0, 0.0};
/* initial rotation speeds of cubes */
static GLfloat w[3] = {0.1, 0.2, 0.3};
#define MY_Z0 3.0
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



static void
normalize(GLfloat v[3])
{
	GLfloat d;
	
	d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	if (d == 0.0) {
		v[0] = d = 1.0;
	}
	d = 1 / d;
	v[0] *= d;
	v[1] *= d;
	v[2] *= d;
}


#define FABS(x) ( (x) > 0.0 ? (x) : -(x) )

static void drawtriangle(GLfloat *v1, GLfloat *v2, GLfloat *v3)
{
	int i;

	/* cool color scheme */
	GLfloat color[3][3];
	for (i = 0; i < 3; i++) {
		color[0][i] = FABS(sin((v1[i])));
		color[1][i] = FABS(cos((v2[i])));
		color[2][i] = tan((v3[i]));
	}

	glBegin(GL_TRIANGLES);
	glNormal3fv(v1); glColor3fv(color[0]); glVertex3fv(v1);
	glNormal3fv(v2); glColor3fv(color[1]); glVertex3fv(v2);
	glNormal3fv(v3); glColor3fv(color[2]); glVertex3fv(v3);
	glEnd();
}




static void
subdivide(GLfloat *v1, GLfloat *v2, GLfloat *v3, long depth)
{
	GLfloat v12[3], v23[3], v31[3];
	GLint i;

	/* this used to be drawtriangle(v1, v2, v3);
	 * but that caused cw triangles, revealing the inside
	 * of our object */
	if (depth == 0) {
		drawtriangle(v2, v1, v3);
		return;
	}

	for (i = 0; i < 3; i++) {
		v12[i] = v1[i] + v2[i];
		v23[i] = v2[i] + v3[i];
		v31[i] = v3[i] + v1[i];
	}
	
	normalize(v12);
	normalize(v23);
	normalize(v31);

	subdivide(v1, v12, v31, depth-1);
	subdivide(v2, v23, v12, depth-1);
 	subdivide(v3, v31, v23, depth-1);
	subdivide(v12, v23, v31, depth-1);
}








static void effect4_reset(void)
{
	int i;
	
	for (i = 0; i < 3; i++) {
		rot[i] = 0.0;
		w[i] = 0.0;
	}
}








static void effect4_display_cb(void)
{
	int i;
	char s[256];

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

	glCallList(list);

#if 0
	for (i = 0; i < 20; i++)
		subdivide(&vdata [ tindices [i][0] ] [0],
			  &vdata [ tindices [i][1] ] [0],
			  &vdata [ tindices [i][2] ] [0],
			  SUBDIVISION_DEPTH);
#endif	

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
		sprintf(s, " ");
		messages_print(s, frame_count, fontcolor);
	}


	mainPostRedisplay();
	mainSwapBuffers();

	/* activate pause if requested */
	if (pause_is_requested())
		pause_activate();

	frame_count++;
}

static void effect4_reshape_cb(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
}

static void effect4_keyboard_cb(SDL_KeyboardEvent key)
{
	switch (key.keysym.sym) {
	case SDLK_SPACE:
		break;
	case SDLK_m:
		messages_toggle();
		break;
	case SDLK_p:
		pause_request();
		break;
	case SDLK_r:
		effect4_reset();
		break;
	case SDLK_x:
		w[2] += 0.1;
		break;
	case SDLK_z:
		w[2] -= 0.1;
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


int effect4_init_cb(struct effect *ep)
{
	int i;
	ep=ep;

	list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	for (i = 0; i < 20; i++)
		subdivide(&vdata [ tindices [i][0] ] [0],
			  &vdata [ tindices [i][1] ] [0],
			  &vdata [ tindices [i][2] ] [0],
			  SUBDIVISION_DEPTH);
	glEndList();

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);


	frame_count = 0;

	return 0;
}



void effect4_cleanup_cb(struct effect *ep)
{
	ep = ep;
	glDeleteLists(list, 1);
	return;
}


int effect4_register(struct effect *ep)
{
	ep->e_init     = effect4_init_cb;
	ep->e_cleanup  = effect4_cleanup_cb;

	ep->e_display  = effect4_display_cb;
	ep->e_reshape  = effect4_reshape_cb;
	ep->e_keyboard = effect4_keyboard_cb;

	ep->e_name     = effect4_name;
	return 0;
}
