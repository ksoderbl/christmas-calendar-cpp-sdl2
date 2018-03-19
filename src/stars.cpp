
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "stars.h"
#include "util.h"

#define STARS_MAXDIST 500.0
#define STARS_MINDIST 200.0
#define STARS_NUM 20000

static GLdouble stars[STARS_NUM][3];
static GLdouble starColors[STARS_NUM][3];

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

		starColors[i][0] = randdouble() * 0.5;
		starColors[i][1] = randdouble() * 0.5;
		starColors[i][2] = randdouble() * 0.5;
	}
}

void stars_draw(void)
{
	int i;
	double r, g, b;

	glPushMatrix();
	/* make sure we are always in the middle */
	/*glTranslatef(0.0, 0.0, my_z);*/
	glBegin(GL_POINTS);
	for (i = 0; i < STARS_NUM; i++) {
		r = starColors[i][0];
		g = starColors[i][1];
		b = starColors[i][2];
		glColor4d(r, g, b, 1.0);
		glVertex3dv(&stars[i][0]);
	}
	glEnd();
	glPopMatrix();
}
