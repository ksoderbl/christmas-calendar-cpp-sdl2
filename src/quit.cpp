#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

#include "calendar.h"
#include "main.h"
#include "quit.h"

/*
 * To use this remember to set orthogonal perspective
 */

#if 0
static void print_string(void *font, string s)
{
	int len, i;

	len = (int) s.length();
	for (i = 0; i < len; i++) {
		// TODO
		//mainBitmapCharacter(font, s[i]);
		;
	}

	cout << s << endl;
}
#endif

void quit_message_print(void)
{
#if 0
	GLint vp[4];
	GLdouble width=0.0, height=0.0;
	GLint msgx, msgy;
	
	glGetIntegerv(GL_VIEWPORT, vp);
	width = (GLdouble) vp[2];
	height = (GLdouble) vp[3];
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	msgx = (vp[2] / 2) - quit_message.length() * 6  ;
	msgy = (vp[3] / 2) - 12;
	
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2i(msgx+2, msgy+2);
	//print_string(GLUT_BITMAP_TIMES_ROMAN_24, quit_message);

	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2i(msgx, msgy);
	//print_string(GLUT_BITMAP_TIMES_ROMAN_24, quit_message);
		
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
#endif
}

