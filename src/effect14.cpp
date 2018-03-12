#include <iostream>
#include <string>

using std::cerr;
using std::string;

#include <GL/gl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "main.h"
#include "texture.h"
#include "vectormath.h"

static string effect14_name = "Euro Stars";
static int frame_count   = 0;

static int draw_normals  = 0;
static int use_texture   = 1;
static int use_lighting  = 1;
static int use_specular  = 1;


#define SMAX       2.4
#define SMIN       0.8
#define SZ         0.3
#define SNPTS      5 /* number of sharp points */
#define SNVERTS    (2*SNPTS+2)
#define SNFACES    (2*SNPTS)

#if DEBUG_LEVEL > 0
#define NUMOBJECTS 2
#else
#define NUMOBJECTS 1
#endif

static GLfloat rot[3] = {0.0, 0.0, 0.0};
static GLfloat w[3]   = {0.0, 0.0, 0.0};
static GLfloat myz     = 28.0;

/* number of lights */
#define LT_N 1
static GLfloat lt_amb  [LT_N][4]   = { { 0.1, 0.1, 0.1, 1.0}, };
static GLfloat lt_diff [LT_N][4]   = { { 1.0, 1.0, 1.0, 1.0}, }; 
static GLfloat lt_spec [LT_N][4]   = { { 1.0, 1.0, 1.0, 1.0}, };
static GLfloat lt_pos  [LT_N][4]   = { { 1.0, 10.0, 5.0, 1.0}, };

#define MAT_STAR 0
#define MAT_BLUE 1
#define MAT_N 2

static GLfloat mat_amb  [MAT_N][4] = {{1.0, 1.0, 1.0, 1.0},
				      {0.0, 0.1, 0.3, 1.0},};
static GLfloat mat_diff [MAT_N][4] = {{1.0, 1.0, 1.0, 1.0},
				      {0.0, 0.1, 0.3, 1.0},};
static GLfloat mat_spec [MAT_N][4] = {{1.0, 1.0, 1.0, 1.0},
				      {0.1, 0.1, 0.1, 1.0},};
static GLfloat mat_shine[MAT_N][1] = { {100.0}, {20.0}};

static int  object = 0;

/* TEXTURES */
#define NUMTEXTURES 1
static texture_t *texture[NUMTEXTURES+1] = { 0, };
static string texfilenames[NUMTEXTURES+1]  =
{
	DATADIR "/envgold.rgb",
	""
};




static GLfloat fontcolor[3] = {1.0, 1.0, 1.0};


static void effect14_reset(void)
{
	int i;
	
	for (i = 0; i < 3; i++) {
		rot[i] = 0.0;
		w[i] = 0.0;
	}

	/*myz = 10.0;*/
	return;
}


static void draw_cube(void)
{
	glBegin(GL_QUADS);
	/* Front Face */
	glNormal3f( 0.0f, 0.0f, 0.5f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	/* Back Face */
	glNormal3f( 0.0f, 0.0f,-0.5f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	/* Top Face */
	glNormal3f( 0.0f, 0.5f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	/* Bottom Face */
	glNormal3f( 0.0f,-0.5f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	/* Right Face */
	glNormal3f( 0.5f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	/* Left Face */
	glNormal3f(-0.5f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glEnd();
}


/* due to lack of time, I won't make the code more elegant */

static GLfloat star_vertices[SNVERTS][3];
static GLfloat star_normals [SNFACES][3];
static GLfloat star_backvertices[SNVERTS][3];
static GLfloat star_backnormals [SNFACES][3];


/* calculate star edges etc. */
static void init_star(void)
{
	int i;
	double angle = 0.0;
	GLfloat r;

	star_vertices[SNFACES][0] = 0.0;
	star_vertices[SNFACES][1] = 0.0;
	star_vertices[SNFACES][2] = SZ;

	for (i = 0; i < SNFACES; i++) {
		r = ((i&1) ? SMIN : SMAX);
		angle = (((double)i)+0.5) * ((double)2*M_PI/(SNFACES));
		star_vertices[i][0] = r * (GLfloat)cos(angle);
		star_vertices[i][1] = r * (GLfloat)sin(angle);
		star_vertices[i][2] = 0.0;
	}
	for (i = 0; i < SNFACES; i++) {
		normcrossprod4f(star_normals[i],
				star_vertices[SNFACES],
				star_vertices[i],
				star_vertices[(i+1)%SNFACES]);
	}


	star_backvertices[SNFACES][0] = 0.0;
	star_backvertices[SNFACES][1] = 0.0;
	star_backvertices[SNFACES][2] = -SZ;

	for (i = 0; i < SNFACES; i++) {
		r = ((i&1) ? SMIN : SMAX);
		angle = (((double)i)+0.5) * ((double)2*M_PI/(SNFACES));
		star_backvertices[i][0] = r * (GLfloat)cos(angle);
		star_backvertices[i][1] = r * (GLfloat)sin(angle);
		star_backvertices[i][2] = 0.0;
	}
	for (i = 0; i < SNFACES; i++) {
		normcrossprod4f(star_backnormals[i],
				star_backvertices[SNFACES],
				star_backvertices[(i+1)%SNFACES],
				star_backvertices[i]);

	}
}

static void draw_star(void)
{
	int i;
	GLfloat (*p)[3] = star_vertices;

	glBegin(GL_TRIANGLES);
	for (i = 0; i < SNFACES; i++) {
		glNormal3fv(star_normals[i]);
		glVertex3fv(p[SNFACES]);
		glVertex3fv(p[i]);
		glVertex3fv(p[(i+1)%SNFACES]);
	}
	glEnd();

	if (draw_normals) {
		GLfloat v[3];
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
		for (i = 0; i < SNFACES; i++) {
			v[0] = (p[SNFACES][0]+p[i][0]+p[(i+1)%SNFACES][0])/3.0;
			v[1] = (p[SNFACES][1]+p[i][1]+p[(i+1)%SNFACES][1])/3.0;
			v[2] = (p[SNFACES][2]+p[i][2]+p[(i+1)%SNFACES][2])/3.0;
			glVertex3fv(v);
			V_ADD(v, v, star_normals[i]);
			glVertex3fv(v);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
	}

	p = star_backvertices;

	glBegin(GL_TRIANGLES);
	for (i = 0; i < SNFACES; i++) {
		glNormal3fv(star_backnormals[i]);
		glVertex3fv(p[SNFACES]);
		glVertex3fv(p[i]);
		glVertex3fv(p[(i+1)%SNFACES]);
	}
	glEnd();

	if (draw_normals) {
		GLfloat v[3];
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
		for (i = 0; i < SNFACES; i++) {
			v[0] = (p[SNFACES][0]+p[i][0]+p[(i+1)%SNFACES][0])/3.0;
			v[1] = (p[SNFACES][1]+p[i][1]+p[(i+1)%SNFACES][1])/3.0;
			v[2] = (p[SNFACES][2]+p[i][2]+p[(i+1)%SNFACES][2])/3.0;
			glVertex3fv(v);
			V_ADD(v, v, star_backnormals[i]);
			glVertex3fv(v);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
	}
}



/* The function called when our window is resized
 * (which shouldn't happen, because we're fullscreen) */
static void effect14_reshape_cb(int w, int h)
{
	if (h == 0)
		h = 1;
	
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
}



/* The function to draw the screendrawing function. */
static void effect14_display_cb(void)
{
	char s[256];
	int i, j;
	
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	/*glDepthFunc(GL_LEQUAL);*/

	if (use_lighting) {
		glLightfv(GL_LIGHT0, GL_POSITION, lt_pos [0]);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
		glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb  [MAT_STAR]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diff [MAT_STAR]);
		glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_spec [MAT_STAR]);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine[MAT_STAR]);
	} else {
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, myz,
		  0.0, 0.0, myz-1.0,
		  0.0, 1.0, 0.0);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	tex_use(texture[0]);

	glPushMatrix();
	glRotatef(rot[0], 1.0, 0.0, 0.0);
	glRotatef(rot[1], 0.0, 1.0, 0.0);
	glRotatef(rot[2], 0.0, 0.0, 1.0);


#define RR1 10.0
#define RR2 10.0
	for (i = 0; i < 12; i++) {
		glPushMatrix();
		glTranslatef(RR1*cos(2*M_PI*((double)i)/12),
			     RR2*sin(2*M_PI*((double)i)/12), 0.0);
		glRotatef(-rot[0], 1.0, 0.0, 0.0);
		glRotatef(-rot[1], 0.0, 1.0, 0.0);
		glRotatef(-rot[2], 0.0, 0.0, 1.0);

		switch(object) {
		case 0:
			draw_star();
			break;
		case 1:
			draw_cube();
			break;
		};
		glPopMatrix();
	}


	for (i = 0; i < 3; i++) {
		rot[i] += w[i];
		if (rot[i] >= 360.0)
			rot[i] -= 360.0;
		if (rot[i] < 0.0)
			rot[i] += 360.0;
	}

	glPopMatrix();

	
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);

	if (use_lighting) {
		glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb  [MAT_BLUE]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diff [MAT_BLUE]);
		glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_spec [MAT_BLUE]);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine[MAT_BLUE]);
	}

	glPushMatrix();
#define NI 12
#define NJ 12
#define IW 5.0
#define JH 4.0
	glTranslatef(-IW*(NI/2.0), -JH*(NJ/2.0), -12.0);
	glNormal3f(0.0, 0.0, 1.0);
	for (j = 0; j < NJ; j++) {
		GLfloat tx, ty1, ty2;
		ty1 = ((GLfloat)j)/(GLfloat)NJ;
		ty2 = ((GLfloat)(j+1))/(GLfloat)NJ;
		glBegin(GL_QUAD_STRIP);
		for (i = 0; i <= NI; i++) {
			tx = ((GLfloat)i)/(GLfloat)NI;
			glTexCoord2f(tx, ty1);
			glVertex3f(tx*IW*NI, ty1*JH*NJ, 0.0);
			glTexCoord2f(tx, ty2);
			glVertex3f(tx*IW*NI, ty2*JH*NJ, 0.0);
		}
		glEnd();
	}
	glPopMatrix();
	
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	if (messages_on() || pause_is_requested()) {
		sprintf(s, " ");
		messages_print(s, frame_count, fontcolor);
	}

	/* activate pause if requested */
	if (pause_is_requested())
		pause_activate();

	frame_count++;

}



static void effect14_keyboard_cb(SDL_KeyboardEvent key)
{
	switch (key.keysym.sym) {
	case SDLK_SPACE:
		object++;
		if(object >= NUMOBJECTS)
			object = 0;
		break;
	case SDLK_l:
		use_lighting = !use_lighting;
		break;
	case SDLK_s:
		use_specular = !use_specular;
		break;
	case SDLK_t:
		use_texture = !use_texture;
		break;
	case SDLK_n:
		draw_normals = !draw_normals;
		break;
	case SDLK_r:
		effect14_reset();
		break;
	case SDLK_x:
		w[2] += 0.1;
		break;
	case SDLK_z:
		w[2] -= 0.1;
		break;
	case SDLK_PAGEUP:
		myz -= 0.5;
		if (myz <= 0.0)
			myz = 0.0;
		break;
	case SDLK_PAGEDOWN:
		myz += 0.5;
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
#if 0		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
#endif
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_LINEAR_MIPMAP_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, tp->tx_fmt, tp->tx_w, tp->tx_h,
			     0, tp->tx_fmt, GL_UNSIGNED_BYTE, tp->tx_data);
		gluBuild2DMipmaps(GL_TEXTURE_2D, tp->tx_fmt, tp->tx_w,
				  tp->tx_h, tp->tx_fmt,
				  GL_UNSIGNED_BYTE, tp->tx_data);
	}

	return 0;
}

int Effect14::init()
{
	if ((init_textures()) < 0) {
		cerr << "effect14: textures failed to init.\n";
		return -1;
	}

	init_star();

	/* Really Nice Perspective Calculations */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glLightfv(GL_LIGHT0, GL_SPECULAR, lt_spec[0]);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lt_amb[0]);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lt_diff[0]);
	glLightfv(GL_LIGHT0, GL_POSITION,lt_pos[0]);
	glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.001);
	glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.001);	

	/* Set The Texture Generation Mode For S To Sphere Mapping */
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	/* Set The Texture Generation Mode For T To Sphere Mapping */
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	effect14_reset();

	return 0;
}



static void cleanup_textures(void)
{
	int i;

	for (i = 0; i < NUMTEXTURES; i++)
		tex_free(texture[i]);
}

void Effect14::cleanup()
{
	cleanup_textures();
}

int effect14_register(struct effect *ep)
{
	ep->e_display  = effect14_display_cb;
	ep->e_reshape  = effect14_reshape_cb;
	ep->e_keyboard = effect14_keyboard_cb;

	ep->e_name     = effect14_name;
	return 0;
}

