#include <stdio.h>
#include <string.h>
#include <time.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "main.h"
#include "stars.h"



static string effect1_name = "Colored Cubes";

//Effect1::Effect1(){}

//Effect1::~Effect1(){}

//string Effect1::name() { return effect1_name; }

/* state variables */

/*static int blend = 1;*/
static int frame_count = 0;      /* number of frames drawn */

static GLuint cube_list;


static GLfloat rot[3] = {0.0, 0.0, 0.0};
/* initial rotation speeds of cubes */
static GLfloat w[3] = {0.1, 0.2, 0.3};
#define MY_Z0 22.0
static GLfloat my_z = MY_Z0;


static GLfloat global_ambient[]  = { 0.1, 0.1, 0.1, 1.0 };

static GLfloat light0_ambient[]  = { 0.1, 0.1, 0.1, 1.0 };
static GLfloat light0_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light0_position[] = { 20.0, 20.0, 20.0, 1.0 };

static GLfloat mat_ambient[]     = { 0.2, 0.2, 0.2, 1.0 };
/*static GLfloat mat_diffuse[]     = { 0.8, 0.8, 0.8, 1.0 };*/
static GLfloat mat_specular[]    = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat mat_emission[]    = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat mat_shininess[]   = { 10.0 };

static GLfloat fontcolor[3] = {1.0, 0.0, 0.0};


static void effect1_reset(void)
{
	int i;
	
	for (i = 0; i < 3; i++) {
		rot[i] = 0.0;
		w[i] = 0.0;
	}
	my_z = MY_Z0;
#if 0	
	w[0] = 90.0;
	w[1] = 90.0;
	w[2] = 90.0;
#endif
}

/*
void Effect1::reset()
{
	int i;
	
	for (i = 0; i < 3; i++) {
		rot[i] = 0.0;
		w[i] = 0.0;
	}
	my_z = MY_Z0;
}
*/

static void draw_cube(void)
{
#if 0
	glDisable(GL_LIGHTING);
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.5);
	glVertex3f(0.0, 0.0, 1.5);
	glEnd();
	glEnable(GL_LIGHTING);	
	glColor4f(1.0, 1.0, 1.0, 1.0);
#endif
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f( 0.5,  0.5,  0.5); /* front face */
	glVertex3f(-0.5,  0.5,  0.5);
	glVertex3f(-0.5, -0.5,  0.5);
	glVertex3f( 0.5, -0.5,  0.5);

	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f( 0.5,  0.5, -0.5); /* right face */
	glVertex3f( 0.5,  0.5,  0.5);
	glVertex3f( 0.5, -0.5,  0.5);
	glVertex3f( 0.5, -0.5, -0.5);

	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(-0.5,  0.5, -0.5); /* back face */
	glVertex3f( 0.5,  0.5, -0.5);
	glVertex3f( 0.5, -0.5, -0.5);
	glVertex3f(-0.5, -0.5, -0.5);


	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-0.5,  0.5,  0.5); /* left face */
	glVertex3f(-0.5,  0.5, -0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(-0.5, -0.5,  0.5);

	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f( 0.5,  0.5, -0.5); /* top face */
	glVertex3f(-0.5,  0.5, -0.5);
	glVertex3f(-0.5,  0.5,  0.5);
	glVertex3f( 0.5,  0.5,  0.5);

	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f( 0.5, -0.5,  0.5); /* bottom face */
	glVertex3f(-0.5, -0.5,  0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f( 0.5, -0.5, -0.5);
	glEnd();

}

static void draw_cubes(int nx, int ny, int nz)
{
	int i, j, k;
	GLfloat x, y, z;
	GLfloat r, g, b;
	GLfloat d;

	/* distance between two adjacent cubes' centers */
	d = 1.5; 
	
	glMatrixMode(GL_MODELVIEW);
				
	for (i = 0; i < nx; i++) {
		for (j = 0; j < ny; j++) {
			for (k = 0; k < nz; k++) {
				glPushMatrix();
				r = ((GLfloat)(nx - i)) / nx;
				g = ((GLfloat)(ny - j)) / ny;
				b = ((GLfloat)(nz - k)) / nz;
				
				x = ((0.5 * nx - i) - 0.5) * d;
				y = ((0.5 * ny - j) - 0.5) * d;
				z = ((0.5 * nz - k) - 0.5) * d;
				
				glTranslatef(x, y, z);
				glColor3f(r, g, b);
				draw_cube();
				
				glPopMatrix();
			}
		}
	}

}




static void effect1_display_cb(void)
{
	int i;
	char s[256];

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	

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

	

	glCallList(cube_list);

	glDisable(GL_LIGHTING);

	stars_draw();

	if (messages_on() || pause_is_requested()) {
		sprintf(s, "w0: %.1f w1: %.1f w2: %.1f", w[0], w[1], w[2]);
		messages_print(s, frame_count, fontcolor);
	}

	/* activate pause if requested */
	if (pause_is_requested())
		pause_activate();

	frame_count++;
}

static void effect1_reshape_cb(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
}

static void effect1_keyboard_cb(SDL_KeyboardEvent key)
{
	switch (key.keysym.sym) {
	case SDLK_r:
		effect1_reset();
		break;
	case SDLK_x:
		w[2] += 0.1;
		break;
	case SDLK_z:
		w[2] -= 0.1;
		break;
	case SDLK_PAGEUP:
		my_z -= 0.5;
		if (my_z <= 0.0)
			my_z = 0.0;
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
	default:
		break;
	}
}

int Effect1::init()
{
	cube_list = glGenLists(1);
	glNewList(cube_list, GL_COMPILE);
	draw_cubes(8, 8, 8);
	glEndList();
	
#if 0
	glShadeModel(GL_FLAT);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
#endif
	glEnable(GL_NORMALIZE);

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);

	glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

	/* global ambient light intensity */
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	
	/* local viewer ? (GL_FALSE is default) */
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	
	/* one-/twosided light model ? (GL_FALSE is default) */
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	/* allow changing of material using glColor */
	/*glColorMaterial(GL_FRONT, GL_AMBIENT);*/
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	/*glColorMaterial(GL_FRONT, GL_SPECULAR);*/
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	frame_count = 0;

	/*effect1_reset();*/
	return 0;
}

void Effect1::cleanup()
{
	glDeleteLists(cube_list, 1);
}

int effect1_register(struct effect *ep)
{
	ep->e_display  = effect1_display_cb;
	ep->e_reshape  = effect1_reshape_cb;
	ep->e_keyboard = effect1_keyboard_cb;

	ep->e_name     = effect1_name;
	return 0;
}
