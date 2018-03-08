
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "stars.h"
#include "util.h"

static GLdouble stars[STARS_NUM][3];

void stars_make(void)
{
	int i;
	GLdouble x, y, z, r;

	for (i = 0; i < STARS_NUM; i++) {
		x = (2.0*randdouble() - 1.0) * STARS_MAXDIST;
		y = (2.0*randdouble() - 1.0) * STARS_MAXDIST;
		z = (2.0*randdouble() - 1.0) * STARS_MAXDIST;
		r = sqrt(x*x + y*y + z*z);
		while (r < STARS_MINDIST) {
			x += 0.1;
			x *= 1.5;
			y *= 1.5;
			z *= 1.5;
			r *= 1.5;
		}
		stars[i][0] = x;
		stars[i][1] = y;
		stars[i][2] = z;
	}
}

void stars_draw(void)
{
	int i;

	glPushMatrix();
	/* make sure we are always in the middle */
	/*glTranslatef(0.0, 0.0, my_z);*/
	glColor4f(0.7, 0.8, 1.0, 1.0);
	glBegin(GL_POINTS);
	for (i = 0; i < STARS_NUM; i++)
		glVertex3dv(&stars[i][0]);
	glEnd();
	glPopMatrix();
}
