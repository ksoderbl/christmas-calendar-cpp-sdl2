
/*
 * Assumes OpenGL version 1.2 for specular textures.
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "main.h"
#include "calendar_primitives.h"
#include "texture.h"
#include "picking.h"
#include "stars.h"
#include "util.h"
#include "vectormath.h"
#include "quit.h"

static string quit_message = "Really Quit ? (y/n)";
static bool quitting = false;

void quit_request(void)
{
	cout << quit_message << "\n";
	quitting = true;
}

static void calendar_reset_hatches(void);

#define RAD2DEG(x) ((180.0 * (x))/M_PI)

#define USE_CW_CULLING 1
#define USE_COORDSYS 0
#define USE_MIPMAPS 0
/*
 * Set this to nonzero if you want to free all textures when changing to
 * another effect.
 */
#define CALENDAR_CLEANUP_TEXTURES 0
/*
 * Increment angles when opening by this amount of degrees per
 * frame
 */
#define ANGLE_INCREMENT 3.0



static int frame_count = 0; /* frames drawn */

string CalendarEffect::getName()
{
	return "Christmas calendar";
}

static int use_texture   = 1;
static int use_lighting  = 1;
static int use_specular  = 1;
static int use_culling   = 1;
#if USE_COORDSYS
static int use_coordsys  = 1;
#endif




static GLfloat fontcolor[3] = {0.5, 0.7, 1.0};





/* viewer position and view direction */
static GLfloat pos[4] = { 6.75, -20.2, -3.9, 1.0 };
static double   phi = 1.87, theta = 0.21;

/* these are calculated from pos, phi and theta */
static GLfloat fwd  [4] = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat up   [4] = { 0.0, 0.0, 1.0, 1.0 };
static GLfloat right[4] = { 0.0, 0.0, 1.0, 1.0 };



/* number of lights */
#define LT_N 1

static GLfloat lt_pos  [LT_N][4]   = { { -5.0, -5.0, -5.0, 0.0}, };
static GLfloat lt_amb  [LT_N][4]   = { { 0.2, 0.2, 0.2, 1.0}, };
static GLfloat lt_diff [LT_N][4]   = { { 1.0, 1.0, 1.0, 1.0}, };
static GLfloat lt_spec [LT_N][4]   = { { 1.0, 1.0, 1.0, 1.0}, };

/* number of materials */
#define MAT_N 1
#define MAT_CALENDAR 0
/* material 0: calendar
 */
static GLfloat mat_amb  [MAT_N][4] = { {1.0, 1.0, 1.0, 1.0}, };
static GLfloat mat_diff [MAT_N][4] = { {1.0, 1.0, 1.0, 1.0}, };
static GLfloat mat_spec [MAT_N][4] = { {1.0, 1.0, 1.0, 1.0}, };
static GLfloat mat_shine[MAT_N][1] = { {30.0}, };
/* static GLfloat white[4]            = { 1.0, 1.0, 1.0, 1.0 };*/
static GLfloat black[4]            = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat global_ambient[4]   = { 0.2, 0.2, 0.2, 1.0 };
/*static GLfloat no_shine[]          = { 0.0 };*/
/*static GLfloat lo_shine[]          = { 5.0 };
  static GLfloat hi_shine[]          = { 100.0 };*/




/* ---------------------------------------------------------------------- */

/* TEXTURES */
#define NUMTEXTURES 4
static int textures_initialized = 0;
static texture_t *texture[NUMTEXTURES+1] = { 0, };
static string texfilenames[NUMTEXTURES+1]  =
{
	DATADIR "/julmust.rgb",
	DATADIR "/numbers.rgb",
	DATADIR "/effects.rgb",
	DATADIR "/tux.rgb",
	""
};




/* ---------------------------------------------------------------------- */

/* hatch width and height */
#define C_NHCOL 6
#define C_NHROW 4
#define C_HW 1.6
#define C_HH 1.0
#define C_HROW(h) ((h)/C_NHCOL)
#define C_HCOL(h) ((h)%C_NHCOL)


/* totals for whole calendar */
#define C_NCOL 14
#define C_NROW 10
#define C_NUMVERT (C_NCOL*C_NROW)
#define C_ROW(i) ((i)/C_NCOL)
#define C_COL(i) ((i)%C_NCOL)
#define C_INDEX(r,c) ((r)*C_NCOL+(c)) 

static int objects_initialized = 0;
static GLfloat calendar_vertices[C_NUMVERT][2];
static GLfloat calendar_texcoords[C_NUMVERT][2];

/* ---------------------------------------------------------------------- */

/* The hatches appear on the calendar in this configuration,
 * the number being the index in the hatches table
 *
 *  18 19 20 21 22 23
 *  12 13 14 15 16 17
 *   6  7  8  9 10 11
 *   0  1  2  3  4  5
 */

static int hatches_initialized = 0;
struct hatch
{
	int      h_open;
	int      h_effect;   /* the number on the hatch */
	double   h_angle;    /* the current angle degrees the hatch has opened,
			      * if not 0.0 > h_open == 1 */
	double   h_maxangle; /* the maximum angle the hatch will open to */
};

/* Hatch no 0 is the hatch in the lower left corner etc.
 * The number on the hatch is the h_effect
 */
static struct hatch hatches[24];

/* ---------------------------------------------------------------------- */

struct state {
	GLsizei   s_width;
	GLsizei   s_height;
	GLdouble  s_fovy;
	GLdouble  s_aspect;
	GLdouble  s_near;
	GLdouble  s_far;
} s;

/* ---------------------------------------------------------------------- */

#if USE_COORDSYS
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

#define NNN (10)
#define NAME2INDEX(n) ((n)-(NNN))
#define INDEX2NAME(n) ((n)+(NNN))

/* the index in hatch array of the next
 * effect to run next, -1 means none */
static int effect_to_run = -1;

static int get_index(int effect)
{
	int i;
	
	for (i = 0; i < 24; i++) {
		if (hatches[i].h_effect == effect)
			return i;
	}

	return -1;
}


static void run_effect(int index)
{
	struct hatch *hp;
	
	hp = &hatches[index];
	cout << "hatch number is " << hp->h_effect << "\n";
	if (effect_to_run != -1) {
		cout << "about to run effect with index " << effect_to_run << "\n";
		return;
	}

	if (hp->h_open) {
		cout << "hatch number is " << hp->h_effect << " is already opened\n";
		return;
	}
	hp->h_open = 1;
	effect_to_run = index;
}


static void process_hit(int name)
{
	int index = NAME2INDEX(name);

	cout << "hit (name, index) = (" << name << ", " << index << ")\n";

	if (index < 0 || index >= 24) {
		cout << "no interesting hit\n";
		return;
	}

	run_effect(index);
}







void draw_hatches(GLenum mode)
{
	int i, effect;
	int hrow, hcol;
	struct hatch *hp;

	if (use_texture)
		tex_use(texture[1]);

	/* draw hatches here */
	for (i = 0; i < 24; i++) {
		glPushMatrix();
		if (mode == GL_SELECT)
			glLoadName(INDEX2NAME(i));

		hp = &hatches[i];
		effect = hp->h_effect;
		hrow = C_HROW(i);
		hcol = C_HCOL(i);

		glTranslatef(C_HW * (2*hcol + 1),
			     C_HH * (2*hrow + 1),
			     0.0);
		/* note rotation at beginning of draw_calendar() */
		glRotatef(-hp->h_angle, 0.0, 1.0, 0.0);

		glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0);
		tx_ll(effect); glVertex3f(0.0, 0.0, 0.0);
		tx_ul(effect); glVertex3f(0.0,  C_HH, 0.0);
		tx_ur(effect); glVertex3f(C_HW, C_HH, 0.0);
		tx_lr(effect); glVertex3f(C_HW, 0.0, 0.0);
		glEnd();

		glPopMatrix();
	}

	/* draw hatch picture */
	if (mode == GL_SELECT)
		return;

	/* draw back sides of hatches here */
	for (i = 0; i < 24; i++) {
		glPushMatrix();

		hp = &hatches[i];
		effect = hp->h_effect;
		hrow = C_HROW(i);
		hcol = C_HCOL(i);
		
		glTranslatef(C_HW * (2*hcol + 1),
			     C_HH * (2*hrow + 1),
			     0.0);
		/* note rotation at beginning of draw_calendar() */
		glRotatef(-hp->h_angle, 0.0, 1.0, 0.0);

		glBegin(GL_QUADS);
		glVertex3f(0.0, 0.0, 0.0);
		/* hack - only one texture coord for the whole poly */
		glTexCoord2f(0.2, 0.2);
		glVertex3f(C_HW, 0.0, 0.0);
		glVertex3f(C_HW, C_HH, 0.0);
		glVertex3f(0.0,  C_HH, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glEnd();

		glPopMatrix();

	}

	if (use_texture)
		tex_use(texture[2]);
	for (i = 0; i < 24; i++) {
		hp = &hatches[i];
		if (!hp->h_open)
			continue;
		glPushMatrix();
		effect = hp->h_effect;
		hrow = C_HROW(i);
		hcol = C_HCOL(i);
		glTranslatef(C_HW * (2*hcol + 1),
			     C_HH * (2*hrow + 1),
			     0.0);
		glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0);
		tx_ll(effect); glVertex3f(0.0, 0.0, 0.0);
		tx_ul(effect); glVertex3f(0.0,  C_HH, 0.0);
		tx_ur(effect); glVertex3f(C_HW, C_HH, 0.0);
		tx_lr(effect); glVertex3f(C_HW, 0.0, 0.0);
		glEnd();
		glPopMatrix();
	}
}

void draw_calendar(GLenum mode)
{
	int row, col, index;

	glColor4f(1.0, 1.0, 1.0, 1.0);
	if (use_texture) {
		glEnable(GL_TEXTURE_2D);
		tex_use(texture[0]);
	}

	if (mode == GL_SELECT)
		glLoadName(86);

	glPushMatrix();
	/* rotate calendar into x-z plane*/
	glRotatef(90.0, 1.0, 0.0, 0.0);
	/* translate to lower left corner of calendar */
	glTranslatef(-((C_NCOL-1.0)/2.0)*C_HW,
		     -((C_NROW-1.0)/2.0)*C_HH, 0.0);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, calendar_vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, calendar_texcoords);

	
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, 1.0);
	for (row = 0; row < (C_NROW-1); row++) {
		for (col = 0; col < (C_NCOL-1); col++) {
			/* leave out places for hatches */
			if ((row & 1) && (col & 1))
				continue;
			
			index = C_INDEX(row, col);
			/* XXX: these are in CW order, but
			 * mesa thinks they are in CCW order ???
			 */
			glArrayElement(index);
			glArrayElement(index+C_NCOL);
			glArrayElement(index+1+C_NCOL);
			glArrayElement(index+1);
		}
	}
	glEnd();
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	draw_hatches(mode);
	glPopMatrix();
	
	glPushMatrix();

	if (use_texture) {
		tex_use(texture[3]);
		glEnable(GL_TEXTURE_2D);
	}
	if (mode == GL_SELECT)
		glLoadName(87);
	
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	/* translate to lower left corner of calendar */
	glTranslatef(-((C_NCOL-1.0)/2.0)*C_HW,
		     -((C_NROW-1.0)/2.0)*C_HH, 0.0);
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(0.0, 0.0, 1.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(0.0, (C_NROW-1)*C_HH, 1.0);
	glTexCoord2f(0.0, 0.0); glVertex3f((C_NCOL-1)*C_HW, (C_NROW-1)*C_HH, 1.0);
	glTexCoord2f(0.0, 1.0); glVertex3f((C_NCOL-1)*C_HW, 0.0, 1.0);

	glNormal3f(-1.0, 0.0, 0.0);
	glTexCoord2f(0.5, 0.5);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, (C_NROW-1)*C_HH, 0.0);
	glVertex3f(0.0, (C_NROW-1)*C_HH, 1.0);
	glVertex3f(0.0, 0.0, 1.0);

	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(0.0,             (C_NROW-1)*C_HH, 1.0);
	glVertex3f(0.0,             (C_NROW-1)*C_HH, 0.0);
	glVertex3f((C_NCOL-1)*C_HW, (C_NROW-1)*C_HH, 0.0);
	glVertex3f((C_NCOL-1)*C_HW, (C_NROW-1)*C_HH, 1.0);

	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f((C_NCOL-1)*C_HW, 0.0, 1.0);
	glVertex3f((C_NCOL-1)*C_HW, (C_NROW-1)*C_HH, 1.0);
	glVertex3f((C_NCOL-1)*C_HW, (C_NROW-1)*C_HH, 0.0);
	glVertex3f((C_NCOL-1)*C_HW, 0.0, 0.0);

	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 1.0);
	glVertex3f((C_NCOL-1)*C_HW, 0.0, 1.0);
	glVertex3f((C_NCOL-1)*C_HW, 0.0, 0.0);
	glEnd();

	glPopMatrix();
}



void CalendarEffect::reset()
{
	calendar_reset_hatches();
}


void draw_scene(GLenum mode)
{
	/* perspective transformation */
	gluPerspective(s.s_fovy, s.s_aspect, s.s_near, s.s_far);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(pos[0], pos[1], pos[2],
		  pos[0]+fwd[0], pos[1]+fwd[1], pos[2]+fwd[2], 
		  up[0], up[1], up[2]);

	if (use_culling) {
#if USE_CW_CULLING
		glFrontFace(GL_CW);
#else
		glFrontFace(GL_CCW);
#endif
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
	} else
		glDisable(GL_CULL_FACE);

	if (use_specular)
		glLightfv(GL_LIGHT0, GL_SPECULAR, lt_spec[0]);
	else
		glLightfv(GL_LIGHT0, GL_SPECULAR, black);
	
	if (use_lighting) {
		glLightfv(GL_LIGHT0, GL_POSITION, lt_pos[0]);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
		glLightfv(GL_LIGHT0, GL_AMBIENT,  lt_amb[0]);
		glLightfv(GL_LIGHT0, GL_DIFFUSE,  lt_diff[0]);
		/* inefficient, but easy */
		glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb  [MAT_CALENDAR]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diff [MAT_CALENDAR]);
		glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_spec [MAT_CALENDAR]);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine[MAT_CALENDAR]);

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);

	} else {
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
	}

	if (use_texture) {
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	draw_calendar(mode);

	if (mode != GL_SELECT) {
		/*draw stars */
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2]);
		stars_draw();
		glPopMatrix();
#if USE_COORDSYS
		if (use_coordsys)
			draw_coordsys();
#endif
	}

}






/* ---------------------------------------------------------------------- */

static unsigned int idle_time = 0;

void calendar_idle_cb(void)
{
	struct hatch *hp;
	int newtime;

	///cout << "calendar_idle_cb(), effect_to_run = " << effect_to_run << endl;

	/* check if we need to run another effect */
	/* this is really ugly, but was programmed in haste */
	if (effect_to_run != -1) {
		hp = &hatches[effect_to_run];
		if (hp->h_angle < hp->h_maxangle) {
			hp->h_angle += ANGLE_INCREMENT;
		} else {
			if (idle_time == 0)
				idle_time = (unsigned int)time(0);
			else {
				newtime = (unsigned int)time(0);

				/* +1 is because the first time might have
				 * been taken just before the counter
				 * increments */
				if (newtime-idle_time >= (CALENDAR_DELAY+1)) {
					main_run_effect(hp->h_effect);
					effect_to_run = -1;
					idle_time = 0;
				}
			}
		}
	}
}

void CalendarEffect::drawFrame()
{
	char s[256];
	double up_phi, up_theta;

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
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
	/* right = fwd x up */
	normcrossprod3f(right, fwd, up);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	draw_scene(GL_RENDER);

	if (messages_on()) {
		sprintf(s,
			"pos={%.2f, %.2f, %.2f} "
			"phi=%.2f theta=%.2f ",
			pos[0], pos[1], pos[2],
			phi, theta);
		messages_print(s, frame_count, fontcolor);
	}

	if (quitting)
		quit_message_print();

	frame_count++;
}

void CalendarEffect::resize(int w, int h)
{
	if (h == 0)
		h = 1;
	s.s_width = (GLsizei)w;
	s.s_height = (GLsizei)h;
	printf("size is %d x %d\n", w, h);

	s.s_aspect = (GLdouble)w/(GLdouble)h;
	glViewport(0, 0, s.s_width, s.s_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(s.s_fovy, s.s_aspect, s.s_near, s.s_far);
}


void CalendarEffect::mouseButtonDownEvent(SDL_MouseButtonEvent button)
{
	int hit;
	int x = button.x;
	int y = button.y;

	if (button.button != SDL_BUTTON_LEFT)
		return;
	if (button.type != SDL_MOUSEBUTTONDOWN)
		return;

	hit = pick_object(x, y, draw_scene);
	process_hit(hit);
}

void CalendarEffect::mouseMotionEvent(SDL_MouseMotionEvent motion)
{
}

void CalendarEffect::keyboardEvent(SDL_KeyboardEvent key)
{
	int i, effect = 0;

	if (quitting) {
		switch (key.keysym.sym) {
		case SDLK_y:
			exit(0);
		case SDLK_n:
		case SDLK_ESCAPE:
			quitting = false;
			break;
		default:
			break;
		}
		return;
	}
  
	switch (key.keysym.sym) {
	case SDLK_l:
		use_lighting = !use_lighting;
		break;
	case SDLK_SPACE:
		break;
	case SDLK_s:
		use_specular = !use_specular;
		break;
	case SDLK_t:
		use_texture = !use_texture;
		break;
	case SDLK_c:
		use_culling = !use_culling;
		break;

	case SDLK_F1:  effect = 1;  break;
	case SDLK_F2:  effect = 2;  break;
	case SDLK_F3:  effect = 3;  break;
	case SDLK_F4:  effect = 4;  break;
	case SDLK_F5:  effect = 5;  break;
	case SDLK_F6:  effect = 6;  break;
	case SDLK_F7:  effect = 7;  break;
	case SDLK_F8:  effect = 8;  break;
	case SDLK_F9:  effect = 9;  break;
	case SDLK_F10: effect = 10; break;
	case SDLK_F11: effect = 11; break;
	case SDLK_F12: effect = 12; break;

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
	
	if (effect != 0) {
		int index;
		Uint16 modifiers;
		
		modifiers = key.keysym.mod;
		if (modifiers & KMOD_SHIFT)
			effect += 12;
		index = get_index(effect);
		run_effect(index);
#if 0
		printf("Proceeding to run effect %d\n", effect);
		main_run_effect(effect);
#endif
	}
}

/* ---------------------------------------------------------------------- */


static int calendar_init_textures(void)
{
	int i;
	texture_t *tp;
/*#if !CALENDAR_CLEANUP_TEXTURES*/
	if (textures_initialized)
		return 0;
/*#endif*/
	for (i = 0; i < NUMTEXTURES; i++) {
		/* set up calendar main texture */
		if (texfilenames[i] == "")
			continue;
		
		texture[i] = tex_init(texfilenames[i]);
		tp = texture[i];
		if (!tp) {
			cerr << "failed to load " << texture[i]->tx_filename << endl;
			return -1;
		}

		/* do we need this ??? */
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				GL_LINEAR);
#if USE_MIPMAPS
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_LINEAR_MIPMAP_LINEAR);
#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_LINEAR);
#endif
		glTexImage2D(GL_TEXTURE_2D, 0, tp->tx_fmt,
			     tp->tx_w, tp->tx_h,
			     0, tp->tx_fmt, GL_UNSIGNED_BYTE, tp->tx_data);
#if USE_MIPMAPS
		gluBuild2DMipmaps(GL_TEXTURE_2D, tp->tx_fmt,
				  tp->tx_w, tp->tx_h,
				  tp->tx_fmt, GL_UNSIGNED_BYTE, tp->tx_data); 
#endif
	}
	printf("textures Initialized\n");
	textures_initialized = 1;
	return 0;
	
	

#if 0	
	tex_fmt_rgb_to_rgba(tex, 0xff);

	p = tex->tx_data;
	for (i = 0; i < tex->tx_w * tex->tx_h; i++) {
		if (p[0] == 0xff && p[1] == 0xff && p[2] == 0xff) {
			p[3] = 0x0;
			foobar++;
		}
		p += 4;
	}

	printf("----> mycount = %d\n", foobar);

	for (i = 0; i < tex->tx_w * tex->tx_h * 3; i++) {
		GLubyte data;
		data = tex->tx_data[i];
		data = 0xff-data;
		tex->tx_data[i] = data;
		/*
		  if (tex->tx_data[i] == 0xff)
		  tex->tx_data[i] = 0x7f;
		  else
		tex->tx_data[i] = 0xff;*/
		/*tex->tx_data[3*i]
		  tex->tx_data[3*i+1]
		  tex->tx_data[3*i+2]*/
	}

#endif
}


#if CALENDAR_CLEANUP_TEXTURES
void calendar_cleanup_textures(void)
{
	int i;

	for (i = 0; i < NUMTEXTURES; i++)
		tex_free(textures[i]);
	
	textures_initialized = 0;
}
#endif




/* ---------------------------------------------------------------------- */

static void calendar_init_objects(void)
{
	int i, row, col;

	if (objects_initialized)
		return;
	
	for (i = 0; i < C_NUMVERT; i++) {
		row = C_ROW(i);
		col = C_COL(i);
		calendar_vertices[i][0]  = ((GLfloat)col)*C_HW;
		calendar_vertices[i][1]  = ((GLfloat)row)*C_HH;
		calendar_texcoords[i][0] = ((GLfloat)col)*(1.0/(C_NCOL-1));
		calendar_texcoords[i][1] = ((GLfloat)row)*(1.0/(C_NROW-1));
	}

	objects_initialized = 1;
}


/* ---------------------------------------------------------------------- */

static int effectorder[24];

static void make_random_order(void)
{
        int i, i1, i2, tmp;
        for (i = 0; i < 24; i++)
                effectorder[i] = i+1;

        for (i = 0; i < 500; i++) {
                i1 = rand() % 24;
                i2 = rand() % 24;
                if (i1 != i2) {
                        tmp = effectorder[i1];
                        effectorder[i1] = effectorder[i2];
                        effectorder[i2] = tmp;
                }
        }
}

static void calendar_reset_hatches(void)
{
	int i, deg;
	struct hatch *hp;
	make_random_order();

	effect_to_run = -1;
	for (i = 0; i < 24; i++) {
		hp = &hatches[i];
		hp->h_open   = 0;
		hp->h_effect = effectorder[i];
		hp->h_angle  = 0.0;
		deg = ((rand() % 50) + 90);
		hp->h_maxangle = deg;
	}
}


static void calendar_init_hatches(void)
{
	if (hatches_initialized)
		return;
	calendar_reset_hatches();
	hatches_initialized = 1;
}


/* ---------------------------------------------------------------------- */

static void calendar_init_lights(void)
{
#if 0
	glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.7);
	glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.01);
	glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01);
#endif	
}

/* ---------------------------------------------------------------------- */

int CalendarEffect::init()
{
	int e;

	idle_time = 0;

	memset (&s, 0, sizeof(struct state));
	s.s_fovy = 60.0;
	s.s_near = 0.1;
	s.s_far  = 1000.0;
#if 0
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
#endif

	e = calendar_init_textures();
	if (e < 0)
		return e;
	calendar_init_objects();
	calendar_init_hatches();
	calendar_init_lights();

	frame_count = 0;
	return 0;
}

void CalendarEffect::cleanup()
{
#if CALENDAR_CLEANUP_TEXTURES
	calendar_cleanup_textures();
#endif
}
