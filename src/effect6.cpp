#include <iostream>
#include <string>

using std::cerr;
using std::string;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "main.h"
#include "texture.h"
#include "vectormath.h"
#include "util.h"


static GLUquadricObj *qp = 0; /* XXX */

string Effect6::getName()
{
	return "Flag";
}

/* state variables */
static int use_texture   = 1;
static int use_lighting  = 1;
static int use_wireframe = 0;
static int use_wind      = 1;
static int use_flat_shading  = 0;
static int draw_normals  = 0;
/* number of frames drawn */
static int frame_count = 0;

static GLfloat rot[3] = {0.0, 0.0, 0.0};

#define MY_Z0 100.0
static GLfloat my_z = MY_Z0;
#define F_POLE_HEIGHT (2.3*MY_Z0)


/* texture */
#define NUMFLAGS 2
#define NUMTEXTURES (NUMFLAGS+2) 


/*static int textures_initialized = 0;*/
static int current_flag = 0; /* current texture index */
static texture_t *textures[NUMTEXTURES+1] = { 0, };

/* first NUMFLAGS textures are flags */
static string texfilenames[NUMTEXTURES+1]  =
{
	DATADIR "/finland.rgb",
	DATADIR "/tux.rgb",
	DATADIR "/cloudysky.rgb",
	DATADIR "/island.rgb",
	""
};





static GLfloat fontcolor[3] = {1.0, 1.0, 1.0};

/* ---------------------------------------------------------------------- */

/* number of columns and rows in flag vertex matrix */
#define F_NC 24   /* "width" */
#define F_NR 19   /* "height" */
#define F_NUMVERT (F_NC*F_NR)
#define F_ROW(i) ((i)/F_NC)
#define F_COL(i) ((i)%F_NC)

#define F_INDEX(r,c) ((r)*F_NC+(c)) 
#define F_D 1.0 /* distance between adjacent vertices */
#define F_WIDTH  ((F_NC-1)*F_D)
#define F_HEIGHT ((F_NR-1)*F_D)


#if 0
/* get index of vertex (row, col)
 * returns -1 for nonexistent vertices */
static int f_index(int row, int col)
{
	if (row < 0)
		return -1;
	if (row >= F_NR)
		return -1;
	if (col < 0)
		return -1;
	if (col >= F_NC)
		return -1;
	return F_INDEX(row, col);
}
#endif




static int f_initialized = 0;

static GLdouble f_vpos    [F_NUMVERT][3];
static GLdouble f_vspeed  [F_NUMVERT][3];
static GLdouble f_vforce  [F_NUMVERT][3];
static GLdouble f_vnormal [F_NUMVERT][3];
static GLdouble f_texcoord  [F_NUMVERT][2];
static GLdouble f_dt   = 0.01;  /* time difference between iterations*/
#define F_1SQRT2 0.707106781  /* 1/sqrt(2) */ 

static GLdouble f_g    = -0.1; /* gravity */
static GLdouble f_fc   = -7.0;  /* spring force constant */
static GLdouble f_damp = 0.990; /* damping constant for velocity */
#define F_WIND0 0.0 /* initial wind, x direction */
#define F_WDELTA (1.0/10.0)  /* wing change size */
#define F_WF    0.0             /* how much the wind affects a flag node
				 * even if the flag is collinear with the wind*/
static GLdouble wind[3] = { F_WIND0, 0.0, 0.0 };
static GLdouble f_maxwind[3] = { 2.0, 0.01, 2.0 };


/* factor to use for wind vector visualisation */
#define F_W 20.0



#if 0
static GLdouble f_g    = -0.05; /* gravity */
static GLdouble f_fc   = -1.0;  /* spring force constant */
static GLdouble f_damp = 0.9; /* damping constant for velocity */
static GLdouble wind[3] = { 1.0, 0.0, 0.0 };
static GLdouble f_maxwind[3] = { 1.0, 0.2, 1.0 };
/* factor to use for wind vector visualisation */
#define F_W 10.0
#endif





/* ---------------------------------------------------------------------- */
/* lighting related stuff */

static GLfloat mat_specular[]    = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat mat_diffuse[]    = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat mat_shininess[]   = { 50.0 }; 

static GLfloat light0_position[] = { 20.0, 20.0, 20.0, 1.0 };
static GLfloat light0_ambient[] =  { 0.8, 0.9, 1.0, 1.0 };
static GLfloat light0_diffuse[] =  { 0.7, 0.8, 1.0, 1.0 };
#if 0
static GLfloat light1_position[] = { -20.0, 20.0, 20.0, 1.0 };
static GLfloat light1_diffuse[] =  { 0.3, 0.4, 1.0, 1.0 };
static GLfloat light2_position[] = { -20.0, 20.0, -20.0, 1.0 };
static GLfloat light2_diffuse[] =  { 0.2, 0.5, 0.9, 1.0 };
#endif
/* ---------------------------------------------------------------------- */




/* get index difference a node
 * right : dir = 0
 * right, up: dir = 1, etc
 * right : dir = 8 
 */
static int indexdiff[] = {  1,
			    1 + F_NC,
			    F_NC,
			    -1 + F_NC, 
			    -1,
			    -1 - F_NC,
			    - F_NC,
			     1 - F_NC };



static void
update_force(void)
{
	int i, j, row, col, index, index2;
	GLdouble d[3], dist;
	GLdouble factor, w_len;
	/*static int foo = 0;*/

	if (use_wind) {
		wind[0] += (GLdouble)((randdouble()*2.0)-1.0)*F_WDELTA;
		wind[1] += (GLdouble)((randdouble()*2.0)-1.0)*0.1*F_WDELTA;
		wind[2] += (GLdouble)((randdouble()*2.0)-1.0)*F_WDELTA;
		for (i = 0; i < 3; i++) {
			if (wind[i] > f_maxwind[i])
				wind[i] *= 0.8;
			if (wind[i] < -f_maxwind[i])
				wind[i] *= 0.8;
		}
	} else {
		wind[0] = wind[1] = wind[2] = 0.0;
	}


#if 0
	/* init forces */
	for (i = 0; i < F_NUMVERT; i++) {
		f_vforce[i][0] = wind[0];
		f_vforce[i][1] = wind[1] + f_g;
		f_vforce[i][2] = wind[2];
	}
#endif


#if 1
	w_len = sqrt(dotprod(wind, wind));
#if 0
	printf("w_len = %f\n", w_len);
	printf("wind   = {%.2f, %.2f, %.2f}\n", wind[0], wind[1], wind[2]);
#endif
		/* calculate the force of wind */
	for (i = 0; i < F_NUMVERT; i++) {
		if (w_len == 0.0)
			factor = 0.0;
		else
			factor = dotprod (&f_vnormal[i][0], wind)
				/ w_len;

		if (factor < 0.0)
			factor = -factor;
#if 0
		printf("wind   = {%f, %f, %f}\n", wind[0], wind[1], wind[2]);
		printf("normal  = {%f, %f, %f}\n",
		       f_vnormal[i][0],
		       f_vnormal[i][1],
		       f_vnormal[i][2]);
		printf("factor  = %f\n", factor);
		printf("dotprod = %f\n", dotprod (&f_vnormal[i][0], wind));
		printf("w_len   = %f\n", w_len);
#endif
		
#if 1
		/*printf(">>>>\n");*/
		/*printf("factor = %f\n", factor);*/
		/*printf("wind   = {%.2f, %.2f, %.2f}\n",
		  wind[0], wind[1], wind[2]);*/
#endif
		factor = ((1.0-F_WF)*factor + F_WF);
		/*printf("factor = %f\n", factor);*/
		f_vforce[i][0] = wind[0] * factor;
		f_vforce[i][1] = wind[1] * factor + f_g;
		f_vforce[i][2] = wind[2] * factor;
#if 0
		f_vforce[i][0] = 0.0;
		f_vforce[i][1] = 0.0;
		f_vforce[i][2] = 0.0;
#endif
	}
#endif


	


	
	/* calculate all diagonal spring forces in the direction
	 * up and right */
	for (col = 0; col < F_NC-1; col++) {
		for (row = 0; row < F_NR-1; row++) {

			index = F_INDEX(row, col);
			index2 = index + indexdiff[1];
			dist = distance(&f_vpos[index][0],
					&f_vpos[index2][0])
				* F_1SQRT2;
			
			for (j = 0; j < 3; j++) {
				d[j] = (f_vpos[index][j] 
					- f_vpos[index2][j])
					* (dist - F_D);
				f_vforce[index ][j] += f_fc * d[j];
				f_vforce[index2][j] -= f_fc * d[j];
			}
		}
	}
	
	/* calculate all diagonal string forces in the direction up and left */
	for (col = 1; col < F_NC; col++) {
		for (row = 0; row < F_NR-1; row++) {

			index = F_INDEX(row, col);
			index2 = index + indexdiff[3];
			dist = distance(&f_vpos[index][0],
					&f_vpos[index2][0])
				* F_1SQRT2;
			
			for (j = 0; j < 3; j++) {
				d[j] = (f_vpos[index][j] 
					- f_vpos[index2][j])
					* (dist - F_D);
				f_vforce[index ][j] += f_fc * d[j];
				f_vforce[index2][j] -= f_fc * d[j];
			}
		}
	}
	
	/* calculate all string forces in the direction up */
	for (col = 0; col < F_NC; col++) {
		for (row = 0; row < F_NR-1; row++) {

			index = F_INDEX(row, col);
			index2 = index + indexdiff[2];
			dist = distance(&f_vpos[index][0],
					&f_vpos[index2][0]);
			
			for (j = 0; j < 3; j++) {
				d[j] = (f_vpos[index][j] 
					- f_vpos[index2][j])
					* (dist - F_D);
				f_vforce[index ][j] += f_fc * d[j];
				f_vforce[index2][j] -= f_fc * d[j];
			}
		}
		
	}
	
	/* calculate all string forces in the direction right */
	for (col = 0; col < F_NC-1; col++) {
		for (row = 0; row < F_NR; row++) {

			index = F_INDEX(row, col);
			index2 = index + indexdiff[0];
			dist = distance(&f_vpos[index][0],
					&f_vpos[index2][0]);
			
			for (j = 0; j < 3; j++) {
				d[j] = (f_vpos[index][j]
					- f_vpos[index2][j])
					* (dist - F_D);
				f_vforce[index ][j] += f_fc * d[j];
				f_vforce[index2][j] -= f_fc * d[j];
			}
		}
		
	}
}


static void update_velocity(void)
{
	int row, col, index;

	/* calculate velocities - ignore stuff attatched to flagpole */
	for (col = 1; col < F_NC; col++) {
			
		for (row = 0; row < F_NR; row++) {
			index = F_INDEX(row, col);
			f_vspeed[index][0] *= f_damp;
			f_vspeed[index][1] *= f_damp;
			f_vspeed[index][2] *= f_damp;

			f_vspeed[index][0] += f_vforce[index][0] * f_dt;
			f_vspeed[index][1] += f_vforce[index][1] * f_dt;
			f_vspeed[index][2] += f_vforce[index][2] * f_dt;
		}
	}
}


static void update_normals(void)
{
	int row, col, index, indexr, indexu, indexd, indexl;

	/* calculate normals */
	for (col = 0; col < F_NC-1; col++) {
		for (row = 0; row < F_NR-1; row++) {
			index = F_INDEX(row, col);
			indexr = index + indexdiff[0];
			indexu = index + indexdiff[2];
			normcrossprod4(&f_vnormal[index][0],
				       &f_vpos   [index][0],
				       &f_vpos   [indexr][0],
				       &f_vpos   [indexu][0]);
		}
	}
	
	/* top row */
	for (col = 0; col < F_NC-1; col++) {
		row = F_NR-1;
		index = F_INDEX(row, col);
		indexr = index + indexdiff[0];
		indexd = index + indexdiff[6];
		normcrossprod4(&f_vnormal[index][0],
			       &f_vpos   [index][0],
			       &f_vpos   [indexd][0],
			       &f_vpos   [indexr][0]);
	}

	/* right column */
	for (row = 0; row < F_NR-1; row++) {
		col = F_NC-1;
		index = F_INDEX(row, col);
		indexu = index + indexdiff[2];
		indexl = index + indexdiff[4];
		normcrossprod4(&f_vnormal[index][0],
			       &f_vpos   [index][0],
			       &f_vpos   [indexu][0],
			       &f_vpos   [indexl][0]);
	}

	/* top right vertex normal*/
	index = F_INDEX(F_NR-1, F_NC-1);
	indexd = index + indexdiff[6];
	indexl = index + indexdiff[4];
	normcrossprod4(&f_vnormal[index][0],
		       &f_vpos   [index][0],
		       &f_vpos   [indexl][0],
		       &f_vpos   [indexd][0]);

}

static void update_position(void)
{
	int row, col, index;

	/* calculate positions - ignore stuff attatched to flagpole */
	for (col = 1; col < F_NC; col++) {
		for (row = 0; row < F_NR; row++) {

			index = F_INDEX(row, col);
			f_vpos[index][0] += f_vspeed[index][0];
			f_vpos[index][1] += f_vspeed[index][1];
			f_vpos[index][2] += f_vspeed[index][2];
		}
	}
	
}






void Effect6::reset()
{
	int i, row, col;
#if 0	
	for (i = 0; i < 3; i++) {
		rot[i] = 0.0;
		/*w[i] = 0.0;*/
	}
	rot[0] = 320.0;
#endif
	for (col = 0; col < F_NC; col++) {
		for (row = 0; row < F_NR; row++) {
			i = F_INDEX(row, col);
			f_vpos [i][0] = col * F_D;
			f_vpos [i][1] = row * F_D;
			f_vpos [i][2] = 0.0;
			f_vspeed [i][0] = 0.0;
			f_vspeed [i][1] = 0.0;
			f_vspeed [i][2] = 0.0;
			/*f_vnormal [i][0] = 0.0;
			  f_vnormal [i][1] = 0.0;
			  f_vnormal [i][2] = 1.0;*/
			f_texcoord [i][0] = col * (1.0/(F_NC-1));
			f_texcoord [i][1] = row * (1.0/(F_NR-1)); 
		}
	}

	update_normals();


}

void Effect6::drawFrame()
{
	int i, row, col;
	char s[256];

	glClearColor(135.0/255.0, 206.0/255.0, 1.0, 1.0); /* SkyBlue1 */
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#if 0
	i = F_INDEX((2*F_NR)/3-1, (2*F_NC)/3-1);
	gluLookAt(0.0, 0.0, my_z,        /* eye */
		  f_vpos[i][0],
		  f_vpos[i][1],
		  f_vpos[i][2],
		  0.0, 1.0, 0.0);        /* up */
#else
	gluLookAt(0.0, 0.0, my_z,        /* eye */
		  0.0, 0.0, my_z - 1.0,  /* ctr */
		  0.0, 1.0, 0.0);        /* up */
#endif
	glRotatef(rot[0], 1.0, 0.0, 0.0);
	glRotatef(rot[1], 0.0, 1.0, 0.0);
	glRotatef(rot[2], 0.0, 0.0, 1.0);
#if 0
	for (i = 0; i < 3; i++) {
		rot[i] += w[i];
	}
#endif

	update_force();
	update_velocity();
	update_position();
	update_normals();

	glColor4f(1.0, 1.0, 1.0, 1.0);


	if (use_flat_shading)
		glShadeModel(GL_FLAT);
	else
		glShadeModel(GL_SMOOTH);

	
	if (use_lighting) {
		glEnable(GL_LIGHTING);
	} else {
		glDisable(GL_LIGHTING);
	}
	
	if (use_texture) {
		tex_use(textures[current_flag]);
		glEnable(GL_TEXTURE_2D);
	}




	glPushMatrix();
	/*glTranslatef(-F_WIDTH/2.0, -F_HEIGHT/2, 0.0);*/




	if (draw_normals) {
		glShadeModel(GL_FLAT);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glColor4f(1.0, 0.0, 0.0, 1.0);			
		glBegin(GL_LINES);
		for (col = 0; col < F_NC; col++) {
			for (row = 0; row < F_NR; row++) {
				i = F_INDEX(row, col);
				glVertex3dv(&f_vpos[i][0]);
				glVertex3f(f_vpos[i][0]+f_vnormal[i][0],
					   f_vpos[i][1]+f_vnormal[i][1],
					   f_vpos[i][2]+f_vnormal[i][2]);
			}
		}
		glEnd();
	}


	
	if (use_flat_shading)
		glShadeModel(GL_FLAT);
	else
		glShadeModel(GL_SMOOTH);
	
	
	if (use_lighting) {
		glEnable(GL_LIGHTING);
	} else {
		glDisable(GL_LIGHTING);
	}
			
	if (use_texture) {
		tex_use(textures[current_flag]);
		glEnable(GL_TEXTURE_2D);
	}
	

	/* draw flag */
	for (col = 0; col < (F_NC-1); col++) {

		if (use_wireframe)
			glBegin(GL_LINE_STRIP);
		else
			glBegin(GL_TRIANGLE_STRIP);
		
		
		for (row = 0; row < (F_NR); row++) {
			i = F_INDEX(row, col);

			glNormal3dv(&f_vnormal    [i][0]);
			glTexCoord2dv(&f_texcoord [i][0]);
			glVertex3dv(&f_vpos       [i][0]);

			glNormal3dv(&f_vnormal    [i+1][0]);
			glTexCoord2dv(&f_texcoord [i+1][0]);
			glVertex3dv(&f_vpos       [i+1][0]);
		}

		glEnd();

	}
	glPopMatrix();



	glShadeModel(GL_FLAT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	/* draw wind vector */
	/*
	if (use_wind) {
		glColor4f(1.0, 0.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex3d(0.0, 1.5*F_HEIGHT, 0.0);
		glVertex3d(F_W*wind[0], F_W*wind[1]+1.5*F_HEIGHT, F_W*wind[2]);
		glEnd();
	}
	*/

	/* pole */
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	glVertex3d(0.0, F_HEIGHT+0.5, 0.0);
	glVertex3d(0.0, -F_POLE_HEIGHT, 0.0);
	glEnd();


	


	/* quadric */
	glColor4f(1.0, 1.0, 1.0, 1.0);

	glPushMatrix();
	/* make sure we are always in the middle */
	/*glTranslatef(pos[0], pos[1], pos[2]);*/
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glRotatef(90.0, 0.0, 0.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	tex_use(textures[NUMFLAGS]);

	gluSphere(qp, F_POLE_HEIGHT*2.0, 50, 20);
	
	glPopMatrix();



#if 1
	glEnable(GL_TEXTURE_2D);
	/* cloud */
	tex_use(textures[NUMFLAGS+1]);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); 
	glVertex3d(-170.0, -F_POLE_HEIGHT, -170.0);
	glTexCoord2d(1.0, 0.0);
	glVertex3d( 170.0, -F_POLE_HEIGHT, -170.0);
	glTexCoord2d(1.0, 1.0);
	glVertex3d( 170.0, -F_POLE_HEIGHT,  170.0);
	glTexCoord2d(0.0, 1.0);
	glVertex3d(-170.0, -F_POLE_HEIGHT,  170.0);
	glEnd();
#endif		


	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);




	if (messages_on() || pause_is_requested()) {
		sprintf(s, "[s]had:%d [t]ex:%d [l]lght:%d"
			" [w]ire:%d [W]ind:%d [f]lag:%d\n",
			use_flat_shading, use_texture, use_lighting,
			use_wireframe, use_wind, current_flag);
		messages_print(s, frame_count, fontcolor);
	}

	/* activate pause if requested */
	if (pause_is_requested())
		pause_activate();

	frame_count++;
}

void Effect6::resize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0);
}

static void effect6_keyboard_cb(SDL_KeyboardEvent key)
{
	switch (key.keysym.sym) {
	case SDLK_s:
		use_flat_shading = !use_flat_shading;
		break;
	case SDLK_l:
		use_lighting = !use_lighting;
		break;
	case SDLK_SPACE:
	case SDLK_f:
		current_flag = (current_flag+1) % NUMFLAGS;
		break;
	case SDLK_t:
		use_texture = !use_texture;
		break;
	case SDLK_w:
		use_wireframe = !use_wireframe;
		break;
	case SDLK_e: // TODO: should be W
		use_wind = !use_wind;
		if (use_wind) {
			wind[0] = F_WIND0;
			wind[1] = wind[2] = 0.0;
		}
		break;
	case SDLK_n:
		draw_normals = !draw_normals;
		break;
	case SDLK_x:
		rot[2] += 10.0;
		break;
	case SDLK_z:
		rot[2] -= 10.0;
		break;
	case SDLK_PAGEUP:
		my_z -= 5.0;
		if (my_z <= 0.0)
			my_z = 0.0;

		break;
	case SDLK_PAGEDOWN:
		my_z += 5.0;
		if (my_z >= 2.2*MY_Z0)
			my_z = 2.2*MY_Z0;

		break;
	case SDLK_LEFT:
		rot[1] -= 10.0;
		break;
	case SDLK_RIGHT:
		rot[1] += 10.0;
		break;
	case SDLK_UP:
		rot[0] -= 10.0;
		break;
	case SDLK_DOWN:
		rot[0] += 10.0;
		break;
	default:
		break;
	}

}

int Effect6::init()
{
	int i;
	texture_t *tex; /* convenience */

	if (f_initialized)
		return 0;

	reset();

#if 0
	for (i = 0; i < 300; i++) {
		printf("updating flag position %d%%\n", i/3);
		update_force();
		update_velocity();
		update_position();
	}
#endif

	for (i = 0; i < NUMTEXTURES; i++) {
		/* set up calendar main texture */
		if (texfilenames[i] == "")
			continue;
		
		textures[i] = tex_init(texfilenames[i]);
		tex = textures[i];
		if (!tex) {
			cerr << "failed to load " << textures[i]->tx_filename << "\n";
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
		glTexImage2D(GL_TEXTURE_2D, 0, tex->tx_fmt,
			     tex->tx_w, tex->tx_h,
			     0, tex->tx_fmt, GL_UNSIGNED_BYTE, tex->tx_data);
#if 0
		gluBuild2DMipmaps(GL_TEXTURE_2D, tex->tx_fmt,
				  tex->tx_w, tex->tx_h,
				  tex->tx_fmt, GL_UNSIGNED_BYTE, tex->tx_data); 
#endif
	}

	current_flag = 0;

	/* lighting */
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
#if 0
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightfv(GL_LIGHT2, GL_DIFFUSE,  light2_diffuse);
#endif
	glEnable(GL_LIGHT0);
#if 0
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
#endif
	/* one-/twosided light model ? (GL_FALSE is default) */
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
#if 0
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
#endif


	/* quadric stuff */
	qp = gluNewQuadric();
	if (!qp) {
		cerr << "could not alloc quadric\n";
		return -1;
	}
	gluQuadricDrawStyle(qp, GLU_FILL);
	gluQuadricOrientation(qp, GLU_INSIDE);
	gluQuadricNormals(qp, GLU_NONE);
	gluQuadricTexture(qp, GL_TRUE);



	f_initialized = 1;



	



#if 0
	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
	/*glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);*/
	glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);

	glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	/* global ambient light intensity */
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	
	/* local viewer ? (GL_FALSE is default) */
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	


	/* allow changing of material using glColor */
	/*glColorMaterial(GL_FRONT, GL_AMBIENT);*/
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	/*glColorMaterial(GL_FRONT, GL_SPECULAR);*/
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
#endif


	frame_count = 0;

	/*effect6_reset();*/
	return 0;
}

void Effect6::cleanup()
{
	int i;

	for (i = 0; i < NUMTEXTURES; i++)
		tex_free(textures[i]);

	gluDeleteQuadric(qp);

	f_initialized = 0;
}

int effect6_register(struct effect *ep)
{
	ep->e_keyboard = effect6_keyboard_cb;

	return 0;
}
