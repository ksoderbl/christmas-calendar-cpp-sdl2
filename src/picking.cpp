#include <iostream>
using std::cerr;

#include <GL/gl.h>
#include <GL/glu.h>

#include <string.h>

#include "main.h"


/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */

#define SBUFSIZE 512


/* The format of the data in the hit buffer is:
 * - number of names on the stack at hit time (should be 1)
 * - min and max window-coordinate z-values in
 *   interval (0,1) scaled by 2^32-1
 * - contents of the name stack at hit time
 * Note: we assume only one name per object
 */

static int process_hits(GLint hits, GLuint buffer[])
{
	int i, numnames, name=-1, closest=-1;
	GLuint *p, minz = 0xffffffff;
	GLuint z0, z1;

	p = (GLuint *)buffer;

	for (i = 0; i < (int)hits; i++) {
		numnames = (int)*p++;
		if (numnames != 1)
			cerr << "process_hits: " << numnames << " names for hit\n";
		z0 = *p++;
		z1 = *p++;
		z1 = z1;
		name = *p++;
		
		if (z0 < minz) {
			minz = z0;
			closest = name;
		}
	}

	return closest;
}


int pick_object(int x, int y, void (*draw_scene)(GLenum mode))
{
	GLuint sbuf[SBUFSIZE];
	GLint hits;
	GLint vp[4];
	GLdouble wx, wy;         /* window x, y */
	GLdouble pw=3.0, ph=3.0; /* pick area width, height in pixels */

	glGetIntegerv(GL_VIEWPORT, vp);
	wx = (GLdouble)x;
	wy = (GLdouble)(vp[3] - y);

	glSelectBuffer(SBUFSIZE, sbuf);
	(void) glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(-1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix(wx, wy, pw, ph, vp);

	draw_scene(GL_SELECT);

	glFlush();

	hits = glRenderMode(GL_RENDER); 
	if (hits <= 0)
		return -1;

	return process_hits(hits, sbuf);
}
