#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

#include <GL/gl.h>
#include <GL/glu.h>

#include <math.h>
#include <time.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "messages.h"

static int messages_are_on = 0;
static char message[256];
static int oldcount;
static clock_t t_old;

static void print_string(void *font, string s);
static double get_fps(int count);






void messages_toggle(void)
{
	messages_are_on = !messages_are_on;
}

int messages_on(void)
{
	return messages_are_on;
}



/*
 * print messages:
 * s    : string to print in lower left corner
 * count: number of frames drawn by effect so far
 *        (updated in display callback) (-1 for no fps calculation)
 * msg_on: are messages enabled
 * pause: 1 if we want to write pause in the upper right corner
 */
void messages_print(char *s, int count, GLfloat fontcolor[3])
{
	GLint vp[4];
	GLint width=0, height=0;
	GLint msgx, msgy;
	//void *font = GLUT_BITMAP_HELVETICA_18;
	void *font = 0;
	string ename = "TODO"; //current_effect->e_name;
	int len=0;
	static double fps = 0.0;

#if 0
	/* XXX */
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
#endif
	glGetIntegerv(GL_VIEWPORT, vp);
	width = (GLdouble) vp[2];
	height = (GLdouble) vp[3];
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, (GLfloat)width, 0.0, (GLfloat)height, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	msgx = 20;
	msgy = 20;
	
	if (messages_are_on) {
		if ((count % FPS_INTERVAL) == 0)
			fps = get_fps(count);
		len = sprintf(message, "fps: %.1f  ", fps);
		sprintf(message + len, s);		

		glColor3fv(fontcolor);
		glRasterPos2i(msgx+1, msgy+1);
		print_string(font, string(message));

		glRasterPos2i(msgx+1, height-msgy+1);
		print_string(font, ename);
	}

	
	if (pause_is_requested()) {
		glColor3fv(fontcolor);
		glRasterPos2i(width-100-msgx+1, height-msgy+1);
		print_string(font, string("pause"));
	}

	/* restore modelview and projection matrices */
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}



/* called from main */
void messages_init_fps(void)
{
	oldcount = 0;
	t_old = 0;
}








static void print_string(void *font, string s)
{
	int len, i;

	len = (int) s.length();
	for (i = 0; i < len; i++) {
		// TODO
		;//mainBitmapCharacter(font, s[i]);
	}

	cout << s << endl;
}


static double get_fps(int count)
{
	clock_t t_new, deltat;
	double t, fps;
	int frames;

	frames = count - oldcount;
	oldcount = count;
	t_new = clock();
	deltat = t_new - t_old;
	t = (double)deltat/(double)CLOCKS_PER_SEC;
	t_old = t_new;
	fps = ((double)frames) / t;

	return fps;
}

