#include <iostream>
#include <string>

using std::cerr;
using std::string;

#include <GL/gl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <math.h>
#include <time.h>


#include "main.h"
#include "stars.h"
#include "util.h"
#include "texture.h"
#include "gauss_table.h"
#include "vectormath.h"

#define USE_DISPLAY_LIST 1

/* roughness for midpoint displacement */
#define ELEC_ROUGHNESS 1.2
/* number of points - 1 in electric discharge, ELEC_N should be 2^n */
#define ELEC_N (256)

static int use_texture   = 1;
static int use_lighting  = 1;
static int use_specular  = 1;

static int light0_on = 1;
static int light1_on = 1;
static int light2_on = 1;

static int frame_count = 0; /* frames drawn */

static string effect20_name = "Electricity";

/* number of lights */
#define LT_N 3
/* number of materials */
#define MAT_N 6

static GLdouble pos[4] = { 10.4, -12.8, 1.0, 1.0 };
static double phi=2.07, theta=0.25;
static GLdouble dir[4] = { 0.0, 0.0, 0.0, 1.0 };
static GLdouble up[4] = { 0.0, 0.0, 1.0, 1.0 };

static GLfloat fontcolor[3] = {0.2, 0.7, 1.0};

static GLfloat lt_amb  [LT_N][4]   = { { 0.2, 0.2, 0.2, 1.0},
				       { 0.3, 0.2, 0.0, 1.0},
				       { 0.2, 0.1, 0.2, 1.0}, };
static GLfloat lt_diff [LT_N][4]   = { { 1.0, 1.0, 1.0, 1.0},
				       { 1.0, 0.7, 0.0, 1.0}, 
				       { 1.0, 0.3, 1.0, 1.0}, };
static GLfloat lt_spec [LT_N][4]   = { { 1.0, 1.0, 1.0, 1.0},
				       { 1.0, 0.9, 0.7, 1.0},
				       { 1.0, 1.0, 1.0, 1.0}, };
static GLfloat lt_pos  [LT_N][4]   = { { 5.0, 5.0, 5.0, 1.0},
				       { 0.0, 0.0, 8.0, 1.0},
				       { 0.0, 0.0, 0.0, 1.0}, };


static GLfloat lt_vel  [LT_N][4]   = { { 0.0, 0.0, 0.0, 1.0}, };
static GLfloat lt_acc  [LT_N][4]   = { { 0.0, 0.0, 0.0, 1.0}, };


/*static GLfloat no_mat[4]           = { 0.0, 0.0, 0.0, 1.0 };*/
  
static GLfloat black[4]            = { 0.0, 0.0, 0.0, 1.0 };
/*static GLfloat no_shine[]          = { 0.0 };*/
/*static GLfloat lo_shine[]          = { 5.0 };
  static GLfloat hi_shine[]          = { 100.0 };*/

#define MAT_FLOOR 0
#define MAT_BASE 1
#define MAT_POLE 2

/*  static GLfloat white[4]            = { 1.0, 1.0, 1.0, 1.0 };*/
/* material 0: floor
 * material 1: base
 * material 2: pole + sphere
 */
static GLfloat mat_amb [MAT_N][4] =  { {0.2, 0.2, 0.2, 1.0},
				       {0.2, 0.2, 0.2, 1.0},
				       {0.2, 0.2, 0.3, 1.0},};
static GLfloat mat_diff [MAT_N][4] = { {0.5, 0.5, 0.5, 1.0},
				       {0.4, 0.4, 0.4, 1.0},
				       {0.4, 0.6, 0.8, 1.0},};
static GLfloat mat_spec [MAT_N][4] = { {0.6, 0.6, 0.6, 1.0},
				       {1.0, 1.0, 1.0, 1.0},
				       {0.8, 0.9, 1.0, 1.0},};
static GLfloat mat_shine[MAT_N][1]  = { {30.0}, {90.0}, {100.0},};

#if USE_DISPLAY_LIST
static GLuint list[3];
#endif

/* list 0: floor
 * list 1: pole */

#define NUMQUADRICS 2
/* quadric 0: floor, base
 * quadric 1: pole and sphere */

/* drawstyles for quadric objects (GLU_POINT||GLU_LINE||GLU_SILHOUETTE||GLU_FILL) */
/* orientation (GLU_OUTSIDE or GLU_INSIDE)    */
/* normals (GLU_NONE, GLU_FLAT or GLU_SMOOTH) */
/* generate tex coords (GL_FALSE or GL_TRUE)  */

static GLUquadricObj *quadric[NUMQUADRICS];
static GLenum q_drawstyle   [] = { GLU_FILL,    GLU_FILL    };
static GLenum q_orientation [] = { GLU_OUTSIDE, GLU_OUTSIDE };
static GLenum q_normals     [] = { GLU_SMOOTH,  GLU_SMOOTH  };
static GLenum q_texcoords   [] = { GL_FALSE,    GL_TRUE     };


/* TEXTURES */
#define NUMTEXTURES 2
static texture_t *texture[NUMTEXTURES+1] = { 0, };
static string texfilenames[NUMTEXTURES+1]  =
{
	DATADIR "/sandstone.rgb",
	DATADIR "/bluestone.rgb",
	""
};




#define N ELEC_N
GLdouble electricity_array[N+1][3];


/* midpoint displacement algorithm */
/* n must be 2^m */
void midpoint_disp(GLdouble (*array)[3], int n, GLdouble xmin, GLdouble xmax,
		   GLdouble roughness)
{
	int i;
	int incr;
	GLdouble (*p)[3];
	GLdouble d;   /* distance between points*/
	GLdouble y1, y2, z1, z2;
	GLdouble sr = roughness; 	/*sr = 0.8;*/

	p = array;

	p[0][1] = 0.0;
	p[n][1] = 0.0;
	p[0][2] = 0.0;
	p[n][2] = 0.0;

	for (i = 0; i <= n; i++)
		p[i][0] = i * ((xmax - xmin)/(GLdouble)n) + xmin;
	
	for (incr = n; incr >= 2; incr >>= 1) {
		d = (GLdouble)incr/(GLdouble)n;
		for (i = (incr>>1); i < n; i += incr) {
			y1 = p[i - (incr>>1)][1];
			y2 = p[i + (incr>>1)][1];
			p[i][1] = 0.5 * (y1 + y2)
				+ d * sr 
				* gaussian_rv[rand() % GAUSS_TABLE_ENTRIES];
			z1 = p[i - (incr>>1)][2];
			z2 = p[i + (incr>>1)][2];
			p[i][2] = 0.5 * (z1 + z2)
				+ d * sr
				* gaussian_rv[rand() % GAUSS_TABLE_ENTRIES];
		}
	}
	

}


static void reset(void)
{
}

#if 0
static void draw_coordsys(void)
{
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(5.0, 0.0, 0.0);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 5.0, 0.0);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 5.0);
	glEnd();
}
#endif

static void make_floor_list(void)
{
	GLdouble inner = 0.0, outer = 20.0;

#if USE_DISPLAY_LIST
	list[0] = glGenLists(1);
	glNewList(list[0], GL_COMPILE);
#endif

	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb  [MAT_FLOOR]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diff [MAT_FLOOR]);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_spec [MAT_FLOOR]);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine[MAT_FLOOR]);

	if (use_texture) {
		glEnable(GL_TEXTURE_2D);
		tex_use(texture[0]);
	}
	gluDisk(quadric[1], inner, outer, 20, 20);

#if USE_DISPLAY_LIST
	glEndList();
#endif
}





#define BASE_HEIGHT 3.0
#define POLE_HEIGHT 8.0 
#define SPHERE_RAD  1.0
#define POLE_X      8.0  /* other pole at -POLE_X */
/* distance tp z-axis from sphere (sphere radius is 1.0) */
#define ELEC_MAX_X (POLE_X-SPHERE_RAD)
#define ELEC_Z (BASE_HEIGHT+POLE_HEIGHT+SPHERE_RAD)

static void make_pole_list(void)
{
	GLdouble bbr = 5.0; /* base bottom rad */
	GLdouble btr = 3.0; /* base top rad */
	GLdouble bh = BASE_HEIGHT;  /* base height */
	GLdouble ph = POLE_HEIGHT;  /* pole height */
	GLdouble sr = SPHERE_RAD;  /* sphere radius */

#if USE_DISPLAY_LIST
	list[1] = glGenLists(1);
	glNewList(list[1], GL_COMPILE);
#endif

	glPushMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb   [MAT_BASE]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diff  [MAT_BASE]);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_spec  [MAT_BASE]);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine [MAT_BASE]);

	if (use_texture) {
		glEnable(GL_TEXTURE_2D);
		tex_use(texture[1]);
	}

	gluCylinder(quadric[1], bbr, btr, bh, 20, 10);
	glTranslatef(0.0, 0.0, bh);
	gluDisk(quadric[1], 0.0, btr, 20, 10);
	
	
	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb  [MAT_POLE]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diff [MAT_POLE]);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_spec [MAT_POLE]);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine[MAT_POLE]);

	glDisable(GL_TEXTURE_2D);
	/* pole and sphere */
	gluCylinder(quadric[0], 0.3, 0.3, ph+sr, 12, 12);

	glTranslatef(0.0, 0.0, ph+sr);
	gluSphere(quadric[0], sr, 20, 12);

	glPopMatrix();

#if USE_DISPLAY_LIST
	glEndList();
#endif
}









/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */

static GLdouble lt[3] = {0.0025, 0.0024, 0.0007};

static void effect20_display_cb(void)
{
	int i, j;
	char s[256];
	double vv;
	/*double side[3];*/
	
	GLdouble (*ea)[3];

	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* calculate current viewing direction */
	dir[0] = cos(phi)*cos(theta);
	dir[1] = sin(phi)*cos(theta);
	dir[2] = sin(theta);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#if 0	
	side[0] = cos(phi - M_PI/2.0);
	side[1] = sin(phi - M_PI/2.0);
	side[2] = 0.0;
	normcrossprod3(up, side, dir);
#endif
	gluLookAt(pos[0], pos[1], pos[2],
		  pos[0]+dir[0], pos[1]+dir[1], pos[2]+dir[2], 
		  up[0], up[1], up[2]);

	/* move lights 0 and 1 */
	for (j = 0; j < 2; j++) {
		vv = sqrt((double)(lt_vel[j][0]  * lt_vel[j][0]
				   +lt_vel[j][1] * lt_vel[j][1]
				   +lt_vel[j][2] * lt_vel[j][2]));
		
		for (i = 0; i < 3; i++) {
			lt_acc[j][i] += (randdouble() - 0.5)*lt[i];
			lt_vel[j][i] *= 1.0/((GLfloat)(vv+4.0));
			lt_vel[j][i] += lt_acc[j][i];
			lt_pos[j][i] += lt_vel[j][i];
		}
#define XY 1.5
		if (lt_pos[j][0] > 5.0)
			lt_acc[j][0] -= randdouble()*lt[0]*XY;
		if (lt_pos[j][0] < -5.0)
			lt_acc[j][0] += randdouble()*lt[0]*XY;
		if (lt_pos[j][1] > 5.0)
			lt_acc[j][1] -= randdouble()*lt[1]*XY;
		if (lt_pos[j][1] < -5.0)
			lt_acc[j][1] += randdouble()*lt[1]*XY;
		if (lt_pos[j][2] > 15.0)
			lt_acc[j][2] -= randdouble()*lt[2]*XY;
		if (lt_pos[j][2] < 8.0)
			lt_acc[j][2] += randdouble()*lt[2]*XY;
	}

	/*draw_coordsys();*/

	glColor4f(1.0, 1.0, 1.0, 1.0);

	if (use_specular) {
		glLightfv(GL_LIGHT0, GL_SPECULAR, lt_spec[0]);
		glLightfv(GL_LIGHT1, GL_SPECULAR, lt_spec[1]);
		glLightfv(GL_LIGHT2, GL_SPECULAR, lt_spec[2]);
	} else {
		glLightfv(GL_LIGHT0, GL_SPECULAR, black);
		glLightfv(GL_LIGHT1, GL_SPECULAR, black);
		glLightfv(GL_LIGHT2, GL_SPECULAR, black);
	}
		


	if (light0_on) {
		glLightfv(GL_LIGHT0, GL_POSITION, lt_pos [0]);
		glEnable(GL_LIGHT0);
	} else
		glDisable(GL_LIGHT0);

	if (light1_on) {
		glLightfv(GL_LIGHT1, GL_POSITION, lt_pos [1]);
		glEnable(GL_LIGHT1);
	} else
		glDisable(GL_LIGHT1);
	

	/* calculate "electricity curve" */
	if (light2_on) {
		ea = electricity_array;
		midpoint_disp(ea, ELEC_N, -ELEC_MAX_X, ELEC_MAX_X,
			      ELEC_ROUGHNESS);
		/* calculate place for light 2 (light from electricity) */
		lt_pos[2][0] = 0.0;
		lt_pos[2][1] = ea[ELEC_N/2][1];
		lt_pos[2][2] = ELEC_Z + ea[ELEC_N/2][2];
		glLightfv(GL_LIGHT2, GL_POSITION, lt_pos [2]);
		glEnable(GL_LIGHT2);
	} else
		glDisable(GL_LIGHT2);
	

	if (use_lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);


	
	/* base */
	glPushMatrix();
	glTranslatef(POLE_X, 0.0, 0.0);
#if USE_DISPLAY_LIST
	glCallList(list[1]);
#else
	make_pole_list();
#endif
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-POLE_X, 0.0, 0.0);
	
#if USE_DISPLAY_LIST
	glCallList(list[1]);
#else
	make_pole_list();
#endif
	glPopMatrix();
	
	
#if USE_DISPLAY_LIST
	glCallList(list[0]);
#else
	make_floor_list();
#endif

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	/* draw stars centered on viewer */
	glPushMatrix();
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glTranslated(pos[0], pos[1], pos[2]);
	stars_draw();
	glPopMatrix();

	/* draw icon for light 0 */
	if (light0_on) {
		glPushMatrix();
		glTranslatef(lt_pos[0][0], lt_pos[0][1], lt_pos[0][2]);
		glColor4fv(lt_diff[0]);
		//TODO mainSolidSphere(0.2, 6, 6);
		glPopMatrix();
	}
	if (light1_on) {
		/* draw icon for light 1 */
		glPushMatrix();
		glTranslatef(lt_pos[1][0], lt_pos[1][1], lt_pos[1][2]);
		glColor4fv(lt_diff[1]);
		//TODO mainSolidSphere(0.2, 6, 6);
		glPopMatrix();
	}

	if (light2_on) {
		glColor4fv(lt_diff[2]);
		glPushMatrix();
		glTranslatef(0.0, 0.0, ELEC_Z);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, electricity_array);
		glDrawArrays(GL_LINE_STRIP, 0, ELEC_N+1);
		glDisableClientState(GL_VERTEX_ARRAY);
		glPopMatrix();
	}

	glColor4f(1.0, 1.0, 1.0, 1.0);
	if (messages_on() || pause_is_requested()) {
		sprintf(s, "1st light %s, 2nd light %s, 3rd light %s",
			light0_on ? " on": "off",
			light1_on ? " on": "off",
			light2_on ? " on": "off");
		messages_print(s, frame_count, fontcolor);
	}

	mainSwapBuffers();

	/* activate pause if requested */
	if (pause_is_requested())
		pause_activate();
	frame_count++;
}



static void effect20_reshape_cb(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
}

static int oldx = 0, oldy = 0;
#define MAX 6
static void effect20_motion_cb(SDL_MouseMotionEvent motion)
{
	int x, y;
	int dx, dy;

	x = motion.x;
	y = motion.y;

	dx = x - oldx;
	dy = y - oldy;

	cout << "dx: " << dx << " xrel: " << motion.xrel << '\n';
	cout << "dy: " << dy << " yrel: " << motion.yrel << '\n';

	phi   -= (GLfloat)dx * (M_PI/360.0);
	theta += (GLfloat)dy * (M_PI/360.0);


	if (theta >= 0.99*(M_PI/2.0))
		theta = 0.99*(M_PI/2.0);
	if (theta <= -0.99*(M_PI/2.0))
		theta = -0.99*(M_PI/2.0);
	if (phi > 2*M_PI)
		phi -= 2*M_PI;
	if (phi < 0)
		phi += 2*M_PI;

	oldx = x;
	oldy = y;
}


static void effect20_keyboard_cb(SDL_KeyboardEvent key)
{
	int i;

	switch (key.keysym.sym) {
	case SDLK_l:
		use_lighting = !use_lighting;
		break;
	case SDLK_1:
		light0_on = !light0_on;
		break;
	case SDLK_2:
		light1_on = !light1_on;
		break;
	case SDLK_SPACE:
	case SDLK_3:
		light2_on = !light2_on;
		break;
	case SDLK_m:
		messages_toggle();
		break;
	case SDLK_s:
		use_specular = !use_specular;
		break;
	case SDLK_p:
		pause_request();
		break;
	case SDLK_r:
		reset();
		break;
#if !USE_DISPLAY_LIST
	case SDLK_t:
		use_texture = !use_texture;
		break;
#endif
	case SDLK_ESCAPE:
		return_to_calendar();
		break;
	case SDLK_END:
		theta = 0.0;
		break;
	case SDLK_PAGEUP:
		theta += (M_PI/30.0);
		break;
	case SDLK_PAGEDOWN:
		theta -= (M_PI/30.0);
		break;
	case SDLK_LEFT:
		phi += 0.1;
		break;
	case SDLK_RIGHT:
		phi -= 0.1;
		break;
	case SDLK_UP:
		for (i = 0; i < 3; i++)
			pos[i] += 0.2 * dir[i];
		break;
	case SDLK_DOWN:
		for (i = 0; i < 3; i++)
			pos[i] -= 0.2 * dir[i];
		break;

	default:
		break;
	}
	
	if (theta >= 0.99*(M_PI/2.0))
		theta = 0.99*(M_PI/2.0);
	if (theta <= -0.99*(M_PI/2.0))
		theta = -0.99*(M_PI/2.0);
	if (phi > 2*M_PI)
		phi -= 2*M_PI;
	if (phi < 0)
		phi += 2*M_PI;
	

	if (pos[2] < 1.0)
		pos[2] = 1.0;
}

static void quadric_error(GLenum error)
{
	error=error;
	cerr << "quadric error: " << (char *)gluErrorString(error) << "\n";
}


static int init_quadrics(void)
{
	int i;
	GLUquadricObj *qp;

	for (i = 0; i < NUMQUADRICS; i++) {
		qp = gluNewQuadric();
		if (!qp)
			return -1;
		quadric[i] = qp;
		gluQuadricCallback(qp, GLU_ERROR, (_GLUfuncptr)quadric_error);
		gluQuadricDrawStyle(qp, q_drawstyle[i]);
		gluQuadricOrientation(qp, q_orientation[i]);
		gluQuadricNormals(qp, q_normals[i]);
		gluQuadricTexture(qp, q_texcoords[i]);
	}
	
	return 0;
}

static void cleanup_quadrics(void)
{
	int i;
	for (i = 0; i < NUMQUADRICS; i++)
		gluDeleteQuadric(quadric[i]);
}


static int init_textures(void)
{
	int i;
	texture_t *tp;

	for (i = 0; i < NUMTEXTURES; i++) {
		if (texfilenames[i] == "")
			continue;
		
		texture[i] = tex_init(texfilenames[i]);
		tp = texture[i];
		if (!tp) {
		  cerr << "failed to load " << texture[i]->tx_filename << "\n";
			return -1;
		}

		/* do we need this ??? */
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, tp->tx_fmt,
			     tp->tx_w, tp->tx_h,
			     0, tp->tx_fmt, GL_UNSIGNED_BYTE, tp->tx_data);
#if 0
		gluBuild2DMipmaps(GL_TEXTURE_2D, tp->tx_fmt,
				  tp->tx_w, tp->tx_h,
				  tp->tx_fmt, GL_UNSIGNED_BYTE, tp->tx_data); 
#endif
	}

	return 0;
}


static void cleanup_textures(void)
{
	int i;

	for (i = 0; i < NUMTEXTURES; i++)
		tex_free(texture[i]);
}

static void init_lights(void)
{
	glLightfv(GL_LIGHT0, GL_AMBIENT,  lt_amb [0]);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  lt_diff[0]);
	glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.7);
	glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.01);
	glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01);
	
	glLightfv(GL_LIGHT1, GL_AMBIENT,  lt_amb [1]);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  lt_diff[1]);
	glLightf( GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.6);
	glLightf( GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.01);
	glLightf( GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01);

	/* electricity */
	glLightfv(GL_LIGHT2, GL_AMBIENT,  lt_amb [2]);
	glLightfv(GL_LIGHT2, GL_DIFFUSE,  lt_diff[2]);
	glLightf( GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.5);
	glLightf( GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.01);
	glLightf( GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.01);
}





static int effect20_init_cb(struct effect *ep)
{
	int e;
	ep=ep;

	if ((e = init_quadrics()) < 0) {
		cerr << "effect20: quadrics failed to init.\n";
		return e;
	}

	if ((e = init_textures()) < 0) {
		cerr << "effect20: textures failed to init.\n";
		return e;
	}

	init_lights();


#if USE_DISPLAY_LIST
	make_floor_list();
	make_pole_list();
#endif

#if 1
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);

#endif

	frame_count = 0;
	return 0;
}


static void effect20_cleanup_cb(struct effect *ep)
{
	ep=ep;
	
	cleanup_quadrics();
	cleanup_textures();
#if USE_DISPLAY_LIST
	glDeleteLists(list[0], 1);
	glDeleteLists(list[1], 1);
#endif


	return;
}


int effect20_register(struct effect *ep)
{
	ep->e_init     = effect20_init_cb;
	ep->e_cleanup  = effect20_cleanup_cb;
	
	ep->e_display  = effect20_display_cb;
        ep->e_reshape  = effect20_reshape_cb;
        ep->e_keyboard = effect20_keyboard_cb;
        ep->e_motion   = effect20_motion_cb;

	ep->e_name     = effect20_name;
	return 0;
}
