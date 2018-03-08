#include <iostream>

using std::cerr;
using std::cout;

#include <GL/gl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <math.h>
#include <time.h>


#include "main.h"
#include "stars.h"

static int blend = 1;
static int frame_count = 0; /* frames drawn */

static string effect3_name = "GLUT Objects";


/*static GLdouble width=0.0, height=0.0;*/
static GLfloat rot[3] = {0.0, 0.0, 0.0};
/* initial rotation speeds */
static GLfloat w[3] = {0.1, 0.1, 0.1};
#define MY_Z0 20.0
static GLfloat my_z = MY_Z0;


static GLfloat fontcolor[3] = {0.5, 0.5, 1.0};


/* light orbit radii */
#define R0 12.5
#define R1 14.5
#define R2 10.5



static int a0[2] = {0}; /* angles for light position */
static int a1[2] = {0}; /* angles for light position */
static int a2[2] = {0}; /* angles for light position */


#define NANGLES 6000
static double *sintable=NULL, *costable=NULL;

static GLdouble angle0=0.0, angle1=0.0, angle2=0.0;



#if 0
GLfloat specular0[]  = {1.0,  .5,  .5, 1.0};
GLfloat specular1[]  = { .5, 1.0,  .5, 1.0};
GLfloat specular2[]  = { .5,  .5, 1.0, 1.0};
GLfloat diffuse0[]   = {1.0, 0.5, 0.2, 1.0};
GLfloat diffuse1[]   = {0.2, 1.0, 0.5, 1.0};
GLfloat diffuse2[]   = {0.5, 0.2, 1.0, 1.0};
#endif


static GLfloat diffuse0[]   = {1.0, 0.0, 0.0, 1.0};
static GLfloat diffuse1[]   = {0.0, 1.0, 0.0, 1.0};
static GLfloat diffuse2[]   = {0.0, 0.0, 1.0, 1.0};
	

static GLfloat lightpos0[4];
static GLfloat lightpos1[4];
static GLfloat lightpos2[4];


static void reset(void)
{
	int i;
	
	for (i = 0; i < 3; i++) {
		rot[i] = 0.0;
		w[i] = 0.0;
	}
	my_z = MY_Z0;
}


static void draw_flare(GLfloat r, GLfloat g, GLfloat b)
{
	glColor4f(r, g, b, 0.6);
	//TODO mainSolidSphere(0.2, 6, 6);
}


static void update_lightpos(void)
{
	int i;

	a0[0] += 5;
	a0[1] += 6;
	
	for (i = 0; i < 2; i++)
		if (a0[i] >= NANGLES)
			a0[i] -= NANGLES;

	a1[0] += 7;
	a1[1] += 4;
	
	for (i = 0; i < 2; i++)
		if (a1[i] >= NANGLES)
			a1[i] -= NANGLES;

	a2[0] += 3;
	a2[1] += 8;
	
	for (i = 0; i < 2; i++)
		if (a2[i] >= NANGLES)
			a2[i] -= NANGLES;

	lightpos0[0] = R0*costable[a0[0]]*costable[a0[1]];
	lightpos0[1] = R0*sintable[a0[0]]*costable[a0[1]];
	lightpos0[2] = R0*sintable[a0[1]];
	lightpos0[3] = 1.0;

	lightpos1[0] = R1*costable[a1[0]]*costable[a1[1]];
	lightpos1[1] = R1*sintable[a1[0]]*costable[a1[1]];
	lightpos1[2] = R1*sintable[a1[1]];
	lightpos1[3] = 1.0;

	lightpos2[0] = R2*costable[a2[0]]*costable[a2[1]];
	lightpos2[1] = R2*sintable[a2[0]]*costable[a2[1]];
	lightpos2[2] = R2*sintable[a2[1]];
	lightpos2[3] = 1.0;
}


static void init(void)
{
	GLfloat mat_spec[]  = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_shine[] = {120.0};
	GLfloat specular0[]  = {1.0, 0.0, 0.0, 1.0};
	GLfloat specular1[]  = {0.0, 1.0, 0.0, 1.0};
	GLfloat specular2[]  = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient0[]   = {0.1, 0.0, 0.0, 1.0};
	GLfloat ambient1[]   = {0.0, 0.1, 0.0, 1.0};
	GLfloat ambient2[]   = {0.0, 0.0, 0.3, 1.0};
	GLfloat ambient[]   = {0.1, 0.1, 0.3, 1.0};

	
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine);
	
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient0);

	glLightfv(GL_LIGHT1, GL_DIFFUSE,  diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
	glLightfv(GL_LIGHT1, GL_AMBIENT,  ambient1);
	
	glLightfv(GL_LIGHT2, GL_DIFFUSE,  diffuse2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular2);
	glLightfv(GL_LIGHT2, GL_AMBIENT,  ambient2);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
#if 0
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
#endif

        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE); /* XXX */
}


static int make_tables(void)
{
	int i;

	sintable = (double *) malloc(NANGLES * sizeof(double));
	if (!sintable) {
		cerr << "effect3: could not allocate space for sintable\n";
		return -1;
	}

	costable = (double *) malloc(NANGLES * sizeof(double));
	if (!costable) {
		free(sintable);
		cerr << "effect3: could not allocate space for costable\n";
		return -1;
	}


	for (i = 0; i < NANGLES; i++) {
		sintable[i] = sin((double)i*2*M_PI/NANGLES);
		costable[i] = cos((double)i*2*M_PI/NANGLES);
	}

	return 0;
}

static void free_tables(void)
{
	if (sintable) {
		free(sintable);
	}
	if (costable) {
		free(costable);
	}
}




static void effect3_display_cb(void)
{
	int i;
	char s[256];

	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
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

	glDisable(GL_LIGHTING);
	stars_draw();

	glEnable(GL_LIGHTING);
	update_lightpos();
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos0);
	glLightfv(GL_LIGHT1, GL_POSITION, lightpos1);
	glLightfv(GL_LIGHT2, GL_POSITION, lightpos2);





#if 1
	glDisable(GL_LIGHTING);

	glPushMatrix(); /* light */
	glTranslatef(lightpos0[0], lightpos0[1], lightpos0[2]);
	draw_flare(diffuse0[0], diffuse0[1], diffuse0[2]);
	glPopMatrix(); /* light */

	glPushMatrix(); /* light */
	glTranslatef(lightpos1[0], lightpos1[1], lightpos1[2]);
	draw_flare(diffuse1[0], diffuse1[1], diffuse1[2]);
	glPopMatrix(); /* light */

	glPushMatrix(); /* light */
	glTranslatef(lightpos2[0], lightpos2[1], lightpos2[2]);
	draw_flare(diffuse2[0], diffuse2[1], diffuse2[2]);
	glPopMatrix(); /* light */

	glEnable(GL_LIGHTING);
#endif

	angle0 += 0.4;
	if (angle0 >= 360.0)
		angle0 -= 360.0;
	angle1 += 0.7;
	if (angle1 >= 360.0)
		angle1 -= 360.0;
	angle2 += 1.1;
	if (angle2 >= 360.0)
		angle2 -= 360.0;


	glPushMatrix();

	glRotatef(angle0, 1.0, 0.0, 0.0);
	glRotatef(angle1, 0.0, 1.0, 0.0);
	glRotatef(angle2, 0.0, 0.0, 1.0);

	//TODO mainSolidSphere(4.0, 20, 20);
#if 0
	//TODO mainSolidTorus(1.6 /*innerRadius*/, 3.0 /* outerRadius*/, 12   /* nsides */, 36  /* rings */);
#endif
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.0, 8.0);
	glRotatef(angle0, 1.0, 0.0, 0.0);
	glRotatef(angle1, 0.0, 1.0, 0.0);
	glRotatef(angle2, 0.0, 0.0, 1.0);
	glScalef(2.0, 2.0, 2.0);
	//TODO mainSolidIcosahedron();
	glPopMatrix();

	glPushMatrix();
	glRotatef(120.0, 0.0, 0.0, 1.0);
	glRotatef(30.0, 0.0, 1.0, 0.0);
	glTranslatef(8.0, 0.0, 0.0);
	glRotatef(angle0, 1.0, 0.0, 0.0);
	glRotatef(angle1, 0.0, 1.0, 0.0);
	glRotatef(angle2, 0.0, 0.0, 1.0);
	/*glScalef(.5, .5, .5);*/
	//TODO mainSolidDodecahedron();
	glPopMatrix();

	glPushMatrix();
	glRotatef(240.0, 0.0, 0.0, 1.0);
	glRotatef(30.0, 0.0, 1.0, 0.0);
	glTranslatef(8.0, 0.0, 0.0);
	glRotatef(angle0, 1.0, 0.0, 0.0);
	glRotatef(angle1, 0.0, 1.0, 0.0);
	glRotatef(angle2, 0.0, 0.0, 1.0);
	glScalef(2.0, 2.0, 2.0);
	//TODO mainSolidOctahedron();
	glPopMatrix();

	
	glPushMatrix();
	/*glRotatef(360.0, 0.0, 0.0, 1.0);*/
	glRotatef(30.0, 0.0, 1.0, 0.0);
	glTranslatef(8.0, 0.0, 0.0);
	glRotatef(angle0, 1.0, 0.0, 0.0);
	glRotatef(angle1, 0.0, 1.0, 0.0);
	glRotatef(angle2, 0.0, 0.0, 1.0);
#if 1
	//TODO mainSolidTorus(1.0 /*innerRadius*/, 2.0 /* outerRadius*/, 12   /* nsides */, 36  /* rings */);
#else
	glScalef(0.8, 1.2, 1.0);
	//TODO mainSolidSphere(2.0 /* radius */, 20   /* slices */, 20   /* stacks */);
#endif
	/*glScalef(2.0, 2.0, 2.0);
	  //TODO mainSolidTetrahedron();*/
	glPopMatrix();

	glDisable(GL_LIGHTING);
	if (messages_on() || pause_is_requested()) {
		sprintf(s, "w0: %.1f w1: %.1f w2: %.1f", w[0], w[1], w[2]);
		messages_print(s, frame_count, fontcolor);
	}

	/* activate pause if requested */
	if (pause_is_requested())
		pause_activate();


	frame_count++;
}



static void effect3_reshape_cb(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
}


static void effect3_keyboard_cb(SDL_KeyboardEvent key)
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




static int effect3_init_cb(struct effect *ep)
{
	ep=ep;

	/*numframes = 0;*/
	if (make_tables() < 0)
		return -1;
	init();
	return 0;
}


static void effect3_cleanup_cb(struct effect *ep)
{
	ep=ep;
	free_tables();
	return;
}


int effect3_register(struct effect *ep)
{
	ep->e_init     = effect3_init_cb;
	ep->e_cleanup  = effect3_cleanup_cb;
	
	ep->e_display  = effect3_display_cb;
        ep->e_reshape  = effect3_reshape_cb;
        ep->e_keyboard = effect3_keyboard_cb;

	ep->e_name     = effect3_name;
	return 0;
}
