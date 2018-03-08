
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#include "main.h"
#include "stars.h"
#include "util.h"

static int blend = 1;
static int light0_on = 1;
static int light1_on = 1;
static int lookat0 = 0;

static int frame_count = 0; /* frames drawn */

static string effect17_name = "Spotlight";

/* number of lights */
#define LT_N 3
/* number of materials */
#define MAT_N 6

static GLdouble pos[4] = { -3.6, 0.0, 19.3, 1.0 };
static GLdouble dir[4] = { 0.0, 0.0, 0.0, 1.0 };
static double theta = -1.36, phi = 0.0, alpha=0.0;


static GLfloat fontcolor[3] = {0.2, 0.3, 0.7};

static GLfloat lt_amb  [LT_N][4]   = { { 0.01, 0.01, 0.2, 1.0},
				       { 0.02, 0.02, 0.01, 1.0}, };
static GLfloat lt_diff [LT_N][4]   = { { 0.1, 0.1, 1.0, 1.0},
				       { 0.9, 0.1, 0.1, 1.0}, };
static GLfloat lt_spec [LT_N][4]   = { { 0.2, 0.4, 1.0, 1.0},
				       { 1.0, 0.8, 0.8, 1.0}, };
static GLfloat lt_pos  [LT_N][4]   = { { 5.0, 5.0, 5.0, 1.0},
				       { 0.0, 0.0, 8.0, 1.0}, };

static GLfloat lt_vel  [LT_N][4]   = { { 0.0, 0.0, 0.0, 1.0}, };
static GLfloat lt_acc  [LT_N][4]   = { { 0.0, 0.0, 0.0, 1.0}, };


static GLfloat spotdir[]           = { 0.0, 0.0, 0.0 };


/*static GLfloat no_mat[4]           = { 0.0, 0.0, 0.0, 1.0 };
  static GLfloat no_shine[]          = { 0.0 };
  static GLfloat lo_shine[]          = { 5.0 };
  static GLfloat hi_shine[]          = { 100.0 };*/
/*static GLfloat black[4]            = { 0.0, 0.0, 0.0, 1.0 };
  static GLfloat white[4]            = { 1.0, 1.0, 1.0, 1.0 };*/
static GLfloat mat_diff [MAT_N][4] = { {0.0, 0.0, 1.0, 1.0},
				       {0.0, 1.0, 0.0, 1.0},
				       {0.6, 0.6, 0.0, 1.0},
				       {0.0, 1.0, 1.0, 1.0},
				       {0.8, 0.7, 0.2, 1.0},};
static GLfloat mat_spec [MAT_N][4] = { {0.4, 0.9, 1.0, 1.0},
				       {0.9, 1.0, 0.9, 1.0},
				       {1.0, 1.0, 0.6, 1.0},
				       {0.8, 1.0, 0.8, 1.0},
				       {0.9, 0.9, 0.7, 1.0},};
static GLfloat mat_amb [MAT_N][4] =  { {0.0, 0.0, 1.0, 1.0},
				       {0.0, 1.0, 0.0, 1.0},
				       {0.6, 0.6, 0.0, 1.0},
				       {0.0, 1.0, 1.0, 1.0},
				       {0.8, 0.7, 0.2, 1.0},};
static GLfloat mat_emis [MAT_N][4] = { {0.0, 0.0, 0.0, 1.0},
				       {0.0, 0.0, 0.0, 1.0},
				       {0.0, 0.0, 0.0, 1.0},
				       {0.5, 0.0, 0.0, 1.0},
				       {0.0, 0.0, 0.0, 1.0},};
static GLfloat mat_shine[MAT_N][1]  = { {120.0}, {10.0}, {15.0}, {50.0}, {50.0}};

static GLuint list;



static void reset(void)
{
}



static void make_list(int make)
{
	int x, y;
	GLdouble xf, yf, dx, dy;

	if (make) {
		list = glGenLists(1);
		glNewList(list, GL_COMPILE);
	}

	glPushMatrix();
	glTranslatef(5.0, 0.0, 1.0);
	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb [0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diff[0]);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_spec[0]);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine[0]);
	glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emis [0]);
	//TODO mainSolidSphere(1.0, 20, 24);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-5.0, 0.0, 1.0);
	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb [1]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diff[1]);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_spec[1]);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine[1]);
	glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emis [1]);
	//TODO mainSolidSphere(1.1, 20, 24);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, -5.0, 1.0);
	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb [2]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diff[2]);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_spec[2]);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine[2]);
	glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emis [2]);
	//TODO mainSolidSphere(1.2, 20, 24);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 5.0, 1.0);
	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb [3]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diff[3]);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_spec[3]);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine[3]);
	glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emis [3]);
	//TODO mainSolidSphere(1.3, 20, 24);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb  [4]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diff [4]);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_spec [4]);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine[4]);
	glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emis [4]);




#define BASESIDE 20.0
#define NN 30
	glPushMatrix();
	glTranslatef(-BASESIDE/2.0, -BASESIDE/2.0, 0.0);
	glNormal3f(0.0, 0.0, 1.0);
	dx = ((GLdouble)BASESIDE)/NN;
	dy = ((GLdouble)BASESIDE)/NN;
	for (x = 0; x <= NN; x++) {
		xf = ((GLfloat)x) * dx;
		glBegin(GL_TRIANGLE_STRIP);
		for (y = 0; y <= NN; y++) {
			yf = ((GLdouble)y) * dy;
			glVertex3d(xf, yf, 0.0);
			glVertex3d(xf+dx, yf, 0.0);
			
		}
		glEnd();

	}
	glPopMatrix();

	if (make) {
		glEndList();
	}
}



static GLdouble lt[3] = {0.00015, 0.00014, 0.00004};

static void effect17_display_cb(void)
{
	int i;
	double vv;
	char s[256];

	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	dir[0] = cos(phi)*cos(theta);
	dir[1] = sin(phi)*cos(theta);
	dir[2] = sin(theta);


	vv = sqrt((double)(lt_vel[0][0]  * lt_vel[0][0]
			   +lt_vel[0][1] * lt_vel[0][1]
			   +lt_vel[0][2] * lt_vel[0][2]));

	for (i = 0; i < 3; i++) {
		lt_acc[0][i] += (randdouble() - 0.5)*lt[i];
		lt_vel[0][i] *= 1.0/((GLfloat)(vv+5.0));
		lt_vel[0][i] += lt_acc[0][i];
		lt_pos[0][i] += lt_vel[0][i];
	}
#define XY 1.8
	if (lt_pos[0][0] > 1.0)
		lt_acc[0][0] -= randdouble()*lt[0]*XY;
	if (lt_pos[0][0] < -1.0)
		lt_acc[0][0] += randdouble()*lt[0]*XY;
	if (lt_pos[0][1] > 1.0)
		lt_acc[0][1] -= randdouble()*lt[1]*XY;
	if (lt_pos[0][1] < -1.0)
		lt_acc[0][1] += randdouble()*lt[1]*XY;
	if (lt_pos[0][2] > 5.0)
		lt_acc[0][2] -= randdouble()*lt[2]*XY;
	if (lt_pos[0][2] < 4.0)
		lt_acc[0][2] += randdouble()*lt[2]*XY;


	
	glLightfv(GL_LIGHT0, GL_AMBIENT,  lt_amb [0]);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  lt_diff[0]);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lt_spec[0]);
	glLightfv(GL_LIGHT0, GL_POSITION, lt_pos [0]);
	glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.06);
	glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.01);
	glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01);


	glLightfv(GL_LIGHT1, GL_AMBIENT,  lt_amb [1]);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  lt_diff[1]);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lt_spec[1]);
	glLightfv(GL_LIGHT1, GL_POSITION, lt_pos [1]);
	glLightf( GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.05);
	glLightf( GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.02);
	glLightf( GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01);
	glLightf( GL_LIGHT1, GL_SPOT_CUTOFF, 25.0);

	/*glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER,
	  local ? GL_TRUE : GL_FALSE);*/
	

	/* spot stuffs */
	alpha += 0.05;
	spotdir[0] = cos(alpha);
	spotdir[1] = sin(alpha);
	spotdir[2] = -1.0;

	/* draw spot light direction vector */
	glColor3fv(lt_diff[1]);
	glBegin(GL_LINES);
	glVertex3fv(lt_pos [1]);
	glVertex3f (lt_pos[1][0]+spotdir[0],
		    lt_pos[1][1]+spotdir[1],
		    lt_pos[1][2]+spotdir[2]);
	glEnd();

	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotdir);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);

	if (light0_on)
		glEnable(GL_LIGHT0);
	else
		glDisable(GL_LIGHT0);
	if (light1_on)
		glEnable(GL_LIGHT1);
	else
		glDisable(GL_LIGHT1);

	glEnable(GL_LIGHTING);



	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (lookat0)
		gluLookAt(pos[0], pos[1], pos[2],
			  (GLdouble)lt_pos[0][0],
			  (GLdouble)lt_pos[0][1],
			  (GLdouble)lt_pos[0][2],
			  0.0, 0.0, 1.0);
	else
		gluLookAt(pos[0], pos[1], pos[2],
			  pos[0]+dir[0], pos[1]+dir[1], pos[2]+dir[2], 
			  0.0, 0.0, 1.0);





	glCallList(list);



	/* draw icon for light 0 */
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslatef(lt_pos[0][0], lt_pos[0][1], lt_pos[0][2]);
	glColor4fv(lt_diff[0]);
	//TODO mainSolidSphere(0.2, 6, 6);
	glPopMatrix();


	stars_draw();
	if (messages_on() || pause_is_requested()) {
		sprintf(s, "pos: %.2f %.2f %.2f phi %.2f theta %.2f ",
			pos[0], pos[1], pos[2], phi, theta);
		messages_print(s, frame_count, fontcolor);
	}

	mainSwapBuffers();

	/* activate pause if requested */
	if (pause_is_requested())
		pause_activate();
	frame_count++;
}



static void effect17_reshape_cb(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
}

static int oldx = 0, oldy = 0;
#define MAX 6
static void effect17_motion_cb(SDL_MouseMotionEvent motion)
{
	int x, y;
	int dx, dy;

	x = motion.x;
	y = motion.y;

	dx = x - oldx;
	dy = y - oldy;

	cout << "dx: " << dx << " xrel: " << motion.xrel << '\n';
	cout << "dy: " << dy << " yrel: " << motion.yrel << '\n';

	if (dx > MAX) dx = MAX;
	if (dy > MAX) dy = MAX;

	phi   -= (GLfloat)dx * (M_PI/360.0);
	theta += (GLfloat)dy * (M_PI/360.0);

	if (theta >= (0.99*M_PI/2.0))
		theta = (0.99*M_PI/2.0);
	if (theta <= -(0.99*M_PI/2.0))
		theta = -(0.99*M_PI/2.0);


	oldx = x;
	oldy = y;
}


#if 0
static void effect17_mouse_cb(int button, int state, int x, int y)
{

}
#endif

// TODO
/*
void effect17_special_cb(int key, int x, int y)
{
	int i;
	x=x;y=y;
	switch (key) {
	case GLUT_KEY_END:
		theta = 0.0;
		break;
	case GLUT_KEY_PAGE_UP:
		theta += (M_PI/30.0);
		break;
	case GLUT_KEY_PAGE_DOWN:
		theta -= (M_PI/30.0);
		break;
	case GLUT_KEY_LEFT:
		phi += 0.1;
		break;
	case GLUT_KEY_RIGHT:
		phi -= 0.1;
		break;
	case GLUT_KEY_UP:
		for (i = 0; i < 3; i++)
			pos[i] += 0.2 * dir[i];
		break;
	case GLUT_KEY_DOWN:
		for (i = 0; i < 3; i++)
			pos[i] -= 0.2 * dir[i];
		break;
	default:
		break;
	}

	if (theta >= (0.99*M_PI/2.0))
		theta = (0.99*M_PI/2.0);
	if (theta <= -(0.99*M_PI/2.0))
		theta = -(0.99*M_PI/2.0);
	
	if (pos[2] < 1.0)
		pos[2] = 1.0;
}
*/

static void effect17_keyboard_cb(SDL_KeyboardEvent key)
{
	switch (key.keysym.sym) {
	case SDLK_1:
		light0_on = !light0_on;
		break;
	case SDLK_2:
		light1_on = !light1_on;
		break;
	case SDLK_3:
		lookat0 = !lookat0;
		break;
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
		break;
	case SDLK_z:
		break;

	case SDLK_ESCAPE:
		return_to_calendar();
		break;
	default:
		break;
	}

}





static int effect17_init_cb(struct effect *ep)
{
	ep=ep;
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);

	make_list(1);


	frame_count = 0;
	return 0;
}


static void effect17_cleanup_cb(struct effect *ep)
{
	ep=ep;
	glDeleteLists(list, 1);
	return;
}


int effect17_register(struct effect *ep)
{
	ep->e_init     = effect17_init_cb;
	ep->e_cleanup  = effect17_cleanup_cb;
	
	ep->e_display  = effect17_display_cb;
        ep->e_reshape  = effect17_reshape_cb;
        ep->e_keyboard = effect17_keyboard_cb;
        ep->e_motion   = effect17_motion_cb;

	ep->e_name     = effect17_name;
	return 0;
}
