#include <stdio.h>
#include <string.h>
#include <time.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "main.h"
#include "stars.h"

string Effect22::getName()
{
	return "GLUT Objects II";
}

/* state variables */

/*static int blend = 1;*/
static int frame_count = 0;      /* number of frames drawn */
#define NUMOBJECTS 6
static int object = 4;
#define NUMCOLORS 8
static int color  = 0;


static GLfloat rot[3] = {0.0, 0.0, 0.0};
/* initial rotation speeds of cubes */
static GLfloat w[3] = {0.5, 0.4, 0.3};
#define MY_Z0 5.0
static GLfloat my_z = MY_Z0;
/* star rotation */
static GLfloat srot[3] = {0.0, 0.0, 0.0};

static GLfloat global_ambient[]  = { 0.0, 0.0, 0.0, 1.0 };

static GLfloat light0_ambient[]  = { 0.1, 0.1, 0.0, 1.0 };
static GLfloat light0_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light0_position[] = { 1.0, 1.0, 15.0, 0.0 };

static GLfloat mat_ambient[]     = { 0.2, 0.2, 0.2, 1.0 };
static GLfloat mat_specular[]    = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat mat_emission[]    = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat mat_shininess[]   = { 30.0 };

static GLfloat fontcolor[3] = {0.3, 0.8, 1.0};

static GLfloat color_array[NUMCOLORS][3] = 
{{ 0.0, 0.0, 0.0 },
 { 0.0, 0.0, 1.0 },
 { 0.0, 1.0, 0.0 },
 { 0.0, 1.0, 1.0 },
 { 1.0, 0.0, 0.0 },
 { 1.0, 0.0, 1.0 },
 { 1.0, 1.0, 0.0 },
 { 1.0, 1.0, 1.0 }};


void Effect22::reset()
{
	int i;
	
	for (i = 0; i < 3; i++) {
		rot[i] = 0.0;
		w[i] = 0.0;
	}
	my_z = MY_Z0;
}

void Effect22::drawFrame()
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


 	glColor3fv(color_array[color]);

	glPushMatrix();
	switch (object) {
	case 0:	//TODO mainSolidTetrahedron();
		break;
	case 1:	//TODO mainSolidCube(1.0);
		break;
	case 2: //TODO mainSolidOctahedron();
		break;
	case 3: 
		glScalef(0.5, 0.5, 0.5);
		//TODO mainSolidDodecahedron();
		break;
	case 4:
		//TODO mainSolidIcosahedron();
		break;
	case 5: //TODO mainSolidTorus(0.4, 0.8, 30, 30);
		break;
	default:
		break;
	}


	glPopMatrix();
	glPopMatrix();


	glDisable(GL_LIGHTING);

	glRotatef(srot[0], 1.0, 0.0, 0.0);
	glRotatef(srot[1], 0.0, 1.0, 0.0);
	glRotatef(srot[2], 0.0, 0.0, 1.0);
	srot[0] += 0.1;
	srot[1] += 0.11;
	srot[2] += 0.12;


	stars_draw();

	if (messages_on() || pause_is_requested()) {
		sprintf(s, "w0: %.1f w1: %.1f w2: %.1f", w[0], w[1], w[2]);
		messages_print(s, frame_count, fontcolor);
	}

	/* activate pause if requested */
	if (pause_is_requested())
		pause_activate();

	/* switch color every 50 frames */
	if (!(frame_count % 50))
		color = (color + 1) % NUMCOLORS;

	frame_count++;
}

void Effect22::resize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
}

static void effect22_keyboard_cb(SDL_KeyboardEvent key)
{
	switch (key.keysym.sym) {
	case SDLK_SPACE:
		object = (object + 1) % NUMOBJECTS;
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

int Effect22::init()
{
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
	/*glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);*/
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);

	glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

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

	object = 4;
	frame_count = 0;

	/*effect22_reset();*/
	return 0;
}

void Effect22::cleanup()
{

}


int effect22_register(struct effect *ep)
{
	ep->e_keyboard = effect22_keyboard_cb;

	return 0;
}
