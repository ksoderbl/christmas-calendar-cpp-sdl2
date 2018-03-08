/*
 * planet effect - written Sun Feb 11 2001
 * Assumes OpenGL version 1.2 for specular textures.
 */

#include <iostream>
#include <string>

using std::cerr;
using std::string;

#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "main.h"
#include "stars.h"
#include "util.h"
#include "texture.h"
#include "vectormath.h"

#define USE_DISPLAY_LISTS 0
#undef DRAW_COORDSYS

static int frame_count = 0; /* frames drawn */
static string effect24_name = "Earth and Sun";
static GLfloat fontcolor[3] = {0.2, 0.7, 1.0};

static int use_texture   = 1;
static int use_lighting  = 1;
static int use_specular  = 1;
static int use_coordsys  = 1;

static int light0_on = 1;

static int do_hack = 1;

/* number of lights */
#define LT_N 1
/* number of materials */
#define MAT_N 1

#define EARTH_R      1.0
#define EARTH_SLICES 72
#define EARTH_STACKS 36
#define EARTH_X      150.0 /* distance from sun */
#define SUN_R        10.0

#define ALPHA_INC    0.00001 /* earth rotation around sun angle speed */
#define BETA_INC     0.0025  /* earth rotation around own axis angle speed */

#define GAMMA2_INC    (0.001)

/* angles are in radians */
static double alpha  = 0.0; /* earth rotation around sun */
static double beta   = 0.0; /* earth rotation around own axis */
static double gamma1 = 0.0; /* sun "halo" rotation 1 */
static double gamma2 = 0.0; /* sun "halo" rotation 2 */
static GLfloat earthpos[4] = { EARTH_X, 0.0, 0.0, 1.0 };
#define RAD2DEG(x) ((180.0 * (x))/M_PI)


static GLfloat pos[4] = { 152.79, 5.17, 0.5, 1.0 };

/* facing sun and earth */
static double   phi = 3.54, theta = 0.0;
/* these 2 can be calculated from pos, phi and theta */
static GLfloat fwd [4] = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat up  [4] = { 0.0, 0.0, 1.0, 1.0 };
static GLfloat right[4] = { 0.0, 0.0, 1.0, 1.0 };

static GLfloat sunpos[4]   = {  50.0, 20.0, 20.0, 1.0 };
/* the sun */

static GLfloat sunamb  [4]   = { 0.0, 0.0, 0.0, 1.0};
static GLfloat sundiff [4]   = { 1.0, 1.0, 1.0, 1.0};
static GLfloat sunspec [4]   = { 1.0, 1.0, 1.0, 1.0};

#if 0
static GLfloat lt_amb  [LT_N][4]   = { { 0.0, 0.0, 0.0, 1.0}, };
static GLfloat lt_diff [LT_N][4]   = { { 1.0, 1.0, 1.0, 1.0}, };
static GLfloat lt_spec [LT_N][4]   = { { 1.0, 1.0, 1.0, 1.0}, };
#endif

/*static GLfloat no_mat[4]           = { 0.0, 0.0, 0.0, 1.0 };*/
  
static GLfloat black[4]            = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat global_ambient[4]   = { 0.05, 0.05, 0.05, 1.0 };
/*static GLfloat no_shine[]          = { 0.0 };*/
/*static GLfloat lo_shine[]          = { 5.0 };
  static GLfloat hi_shine[]          = { 100.0 };*/

#define MAT_PLANET 0

/*  static GLfloat white[4]            = { 1.0, 1.0, 1.0, 1.0 };*/
/* material 0: floor
 * material 1: base
 */
static GLfloat mat_amb  [MAT_N][4] = { {0.2, 0.2, 0.2, 1.0}, };
static GLfloat mat_diff [MAT_N][4] = { {0.5, 0.5, 0.5, 1.0}, };
static GLfloat mat_spec [MAT_N][4] = { {0.6, 0.6, 0.6, 1.0}, };
static GLfloat mat_shine[MAT_N][1] = { {30.0}, };

#if USE_DISPLAY_LISTS
static GLuint list[1];
#endif


#define NUMQUADRICS 1
/* quadric 0: planet */

/* drawstyles for quadric objects
 * (GLU_POINT||GLU_LINE||GLU_SILHOUETTE||GLU_FILL) */
/* orientation (GLU_OUTSIDE or GLU_INSIDE)    */
/* normals (GLU_NONE, GLU_FLAT or GLU_SMOOTH) */
/* generate tex coords (GL_FALSE or GL_TRUE)  */

static GLUquadricObj *quadric[NUMQUADRICS];
static GLenum q_drawstyle   [] = { GLU_FILL    };
static GLenum q_orientation [] = { GLU_OUTSIDE };
static GLenum q_normals     [] = { GLU_SMOOTH  };
static GLenum q_texcoords   [] = { GL_TRUE     };


/* TEXTURES */
#define NUMTEXTURES 8
static texture_t *texture[NUMTEXTURES+1] = { 0, };
static string texfilenames[NUMTEXTURES+1]  =
{
	DATADIR "/earth.rgb",
	DATADIR "/star1.rgb",
	DATADIR "/star2.rgb",
	DATADIR "/lens1.rgb",
	DATADIR "/lens2.rgb",
	DATADIR "/lens3.rgb",
	DATADIR "/lens4.rgb",
	DATADIR "/lens5.rgb",
	""
};



static void reset(void)
{
}

#if DRAW_COORDSYS
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

static void earth(void)
{
#if USE_DISPLAY_LISTS
	list[0] = glGenLists(1);
	glNewList(list[0], GL_COMPILE);
#endif

	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb  [MAT_PLANET]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diff [MAT_PLANET]);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_spec [MAT_PLANET]);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine[MAT_PLANET]);

	if (use_texture) {
		glEnable(GL_TEXTURE_2D);
		tex_use(texture[0]);
	}
	gluSphere(quadric[0], EARTH_R, EARTH_SLICES, EARTH_STACKS);

#if USE_DISPLAY_LISTS
	glEndList();
#endif
}




/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */

static void effect24_display_cb(void)
{
	char s[256];
	double up_phi, up_theta;
	GLfloat sv[4][3]; /* sun texture vertices */
	GLfloat left[3], down[3];
	GLfloat lv[3], rv[3], uv[3], dv[3];

	GLfloat tmp1[3];
	GLfloat tmp2[3];
	GLfloat tmprv[3];
	GLfloat tmpuv[3];

	
	/*double side[3];*/
	
	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* calculate current viewing direction */
	fwd[0] = cos(phi)*cos(theta);
	fwd[1] = sin(phi)*cos(theta);
	fwd[2] = sin(theta);

	/* calculate up vector */
	if (theta >= 0) {
		up_phi = phi + M_PI;
		up_theta = M_PI/2.0 - theta;
	} else {
		up_phi = phi;
		up_theta = theta + M_PI/2.0;
	}
	up[0] = cos(up_phi)*cos(up_theta);
	up[1] = sin(up_phi)*cos(up_theta);
	up[2] = sin(up_theta);
	V_ASSIGN_NEGATION(down, up);

	/* right = fwd x up */
	normcrossprod3f(right, fwd, up);
	V_ASSIGN_NEGATION(left, right);



	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(pos[0], pos[1], pos[2],
		  pos[0]+fwd[0], pos[1]+fwd[1], pos[2]+fwd[2], 
		  up[0], up[1], up[2]);
	glColor4f(1.0, 1.0, 1.0, 1.0);

	if (use_specular)
		glLightfv(GL_LIGHT0, GL_SPECULAR, sunspec);
	else
		glLightfv(GL_LIGHT0, GL_SPECULAR, black);

	if (light0_on) {
		glLightfv(GL_LIGHT0, GL_POSITION, sunpos);
		glEnable(GL_LIGHT0);
	} else
		glDisable(GL_LIGHT0);

	if (use_lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);


	
	
	/* draw the Earth */
	glPushMatrix();

	earthpos[0] = EARTH_X * cos(alpha);
	earthpos[1] = EARTH_X * sin(alpha);
	earthpos[2] = 0.0;
	glTranslatef(earthpos[0], earthpos[1], earthpos[2]);
	glRotatef(-23.0, 0.0, 1.0, 0.0);
	glRotatef((GLfloat)RAD2DEG(beta), 0.0, 0.0, 1.0);
#if USE_DISPLAY_LISTS
	glCallList(list[0]);
#else
	earth();
#endif
	glPopMatrix();


	/*draw stars */
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
	stars_draw();
	glPopMatrix();
#if DRAW_COORDSYS
	if (use_coordsys)
		draw_coordsys();
#endif
	glEnable(GL_TEXTURE_2D);
	/*glDisable(GL_DEPTH_TEST);*/
	glDepthMask(GL_FALSE);  /* XXX */


	/* draw sun */
	tex_use(texture[1]);

	/* rotate coordinate system for sun textures */

	/* halo 1 */
#if 0
	V_ASSIGN(lv, left);
	V_ASSIGN(rv, right);
	V_ASSIGN(uv, up);
	V_ASSIGN(dv, down);
#endif
	
	V_CONSTMUL2(lv, left, SUN_R);
	V_CONSTMUL2(rv, right, SUN_R);
	V_CONSTMUL2(uv, up, SUN_R);
	V_CONSTMUL2(dv, down, SUN_R);

	V_CONSTMUL2(tmp1, rv, (GLfloat)cos(gamma1));
	V_CONSTMUL2(tmp2, uv, (GLfloat)sin(gamma1));
	V_ADD(tmprv, tmp1, tmp2);
	V_CONSTMUL2(tmp1, rv, (GLfloat)(-sin(gamma1)));
	V_CONSTMUL2(tmp2, uv, (GLfloat)cos(gamma1));
	V_ADD(tmpuv, tmp1, tmp2);
	V_ASSIGN(rv, tmprv);
	V_ASSIGN(uv, tmpuv);
	V_ASSIGN_NEGATION(lv, rv);
	V_ASSIGN_NEGATION(dv, uv);
	
	V_ADD3(sv[0], sunpos, dv, lv);
	V_ADD3(sv[1], sunpos, dv, rv);
	V_ADD3(sv[2], sunpos, uv, rv);
	V_ADD3(sv[3], sunpos, uv, lv);
	
	glColor4f(1.0, 1.0, 0.5, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3fv(sv[0]); 
	glTexCoord2f(1.0, 0.0); glVertex3fv(sv[1]); 
	glTexCoord2f(1.0, 1.0); glVertex3fv(sv[2]); 
	glTexCoord2f(0.0, 1.0); glVertex3fv(sv[3]);
	glEnd();

	/* halo 2 */

	if (do_hack) {
		tex_use(texture[2]);
#define M 0.8
		V_CONSTMUL2(lv, left, M*SUN_R);
		V_CONSTMUL2(rv, right, M*SUN_R);
		V_CONSTMUL2(uv, up, M*SUN_R);
		V_CONSTMUL2(dv, down, M*SUN_R);
#undef M	
		V_CONSTMUL2(tmp1, rv, (GLfloat)cos(gamma2));
		V_CONSTMUL2(tmp2, uv, (GLfloat)sin(gamma2));
		V_ADD(tmprv, tmp1, tmp2);
		V_CONSTMUL2(tmp1, rv, (GLfloat)(-sin(gamma2)));
		V_CONSTMUL2(tmp2, uv, (GLfloat)cos(gamma2));
		V_ADD(tmpuv, tmp1, tmp2);
		V_ASSIGN(rv, tmprv);
		V_ASSIGN(uv, tmpuv);
		V_ASSIGN_NEGATION(lv, rv);
		V_ASSIGN_NEGATION(dv, uv);
	
		V_ADD3(sv[0], sunpos, dv, lv);
		V_ADD3(sv[1], sunpos, dv, rv);
		V_ADD3(sv[2], sunpos, uv, rv);
		V_ADD3(sv[3], sunpos, uv, lv);
		
		glColor4f(1.0, 1.0, 0.8, 0.6);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3fv(sv[0]); 
		glTexCoord2f(1.0, 0.0); glVertex3fv(sv[1]); 
		glTexCoord2f(1.0, 1.0); glVertex3fv(sv[2]); 
		glTexCoord2f(0.0, 1.0); glVertex3fv(sv[3]);
		glEnd();
	}


	
	glDepthFunc(GL_LESS);	

	glDepthMask(GL_TRUE);  /* XXX */
	/*glEnable(GL_DEPTH_TEST);*/
	glDisable(GL_BLEND);


	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);


	glColor4f(1.0, 1.0, 1.0, 1.0);
	if (messages_on() || pause_is_requested()) {
		sprintf(s,
			"pos={%.2f, %.2f, %.2f} "
			"phi=%.2f theta=%.2f ",
			pos[0], pos[1], pos[2],
			phi, theta);
		messages_print(s, frame_count, fontcolor);
	}

	mainSwapBuffers();

	/* rotate around sun */
	alpha += ALPHA_INC;
	if (alpha > 2.0*M_PI)
		alpha -= 2.0*M_PI;
	if (alpha < 0.0)
		alpha += 2.0*M_PI;

	/* rotate earth */
	beta += BETA_INC;
	if (beta > 2.0*M_PI)
		beta -= 2.0*M_PI;
	if (beta < 0.0)
		beta += 2.0*M_PI;

	/* rotate halo */
	gamma2 += GAMMA2_INC;
	if (gamma2 > 2.0*M_PI)
		gamma2 -= 2.0*M_PI;
	if (gamma2 < 0.0)
		gamma2 += 2.0*M_PI;




	/* activate pause if requested */
	if (pause_is_requested())
		pause_activate();
	frame_count++;
}



static void effect24_reshape_cb(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
}

static int oldx = 0, oldy = 0;
#define MAX 6
static void effect24_motion_cb(SDL_MouseMotionEvent motion)
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


	if (theta >= (M_PI/2.0))
		theta = (M_PI/2.0);
	if (theta <= -(M_PI/2.0))
		theta = -(M_PI/2.0);
	if (phi > 2*M_PI)
		phi -= 2*M_PI;
	if (phi < 0)
		phi += 2*M_PI;

	oldx = x;
	oldy = y;
}


static void effect24_keyboard_cb(SDL_KeyboardEvent key)
{
	int i;
	
	switch (key.keysym.sym) {
	case SDLK_l:
		use_lighting = !use_lighting;
		break;
	case SDLK_1:
		light0_on = !light0_on;
		break;
	case SDLK_SPACE:
		break;
	case SDLK_h:
		do_hack = !do_hack;
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
	case SDLK_c:
		use_coordsys = !use_coordsys;
		break;
	case SDLK_t:
		use_texture = !use_texture;
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
			pos[i] += 0.2 * fwd[i];
		break;
	case SDLK_DOWN:
		for (i = 0; i < 3; i++)
			pos[i] -= 0.2 * fwd[i];
		break;
		
	case SDLK_ESCAPE:
		return_to_calendar();
		break;
	default:
		break;
	}

	if (theta >= (M_PI/2.0))
		theta = (M_PI/2.0);
	if (theta <= -(M_PI/2.0))
		theta = -(M_PI/2.0);
	if (phi > 2*M_PI)
		phi -= 2*M_PI;
	if (phi < 0)
		phi += 2*M_PI;
	
#if 0
	if (pos[2] < 1.0)
		pos[2] = 1.0;
#endif
}

static void quadric_error(GLenum error)
{
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
		/* set up calendar main texture */
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
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  sunamb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  sundiff);
#if 0
	glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.7);
	glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.01);
	glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01);
#endif	
}





static int effect24_init_cb(struct effect *ep)
{
	int e;
	ep=ep;

	if ((e = init_quadrics()) < 0) {
		cerr << "effect24: quadrics failed to init.\n";
		return  e;
	}

	if ((e = init_textures()) < 0) {
		cerr << "effect24: textures failed to init.\n";
		return  e;
	}

	init_lights();


#if USE_DISPLAY_LISTS
	earth();
#endif

#if 1
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);

#endif

	frame_count = 0;
	return 0;
}


static void effect24_cleanup_cb(struct effect *ep)
{
	ep=ep;
	cleanup_quadrics();
	cleanup_textures();
#if USE_DISPLAY_LISTS
	glDeleteLists(list[0], 1);
	glDeleteLists(list[1], 1);
#endif


	return;
}


int effect24_register(struct effect *ep)
{
	ep->e_init     = effect24_init_cb;
	ep->e_cleanup  = effect24_cleanup_cb;
	
	ep->e_display  = effect24_display_cb;
        ep->e_reshape  = effect24_reshape_cb;
        ep->e_keyboard = effect24_keyboard_cb;
        ep->e_motion   = effect24_motion_cb;

	ep->e_name     = effect24_name;
	return 0;
}
