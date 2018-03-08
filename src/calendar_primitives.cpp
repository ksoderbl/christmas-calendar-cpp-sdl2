#include "main.h"

void draw_triangle (GLfloat x1, GLfloat y1,  GLfloat x2, GLfloat y2,
		    GLfloat x3, GLfloat y3,  GLfloat z)
{
	glBegin(GL_TRIANGLES);
	glVertex3f(x1, y1, z);
	glVertex3f(x2, y2, z);
	glVertex3f(x3, y3, z);
	glEnd();
}


void draw_view_volume (GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, 
		       GLfloat z1, GLfloat z2)
{
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(x1, y1, -z1);
	glVertex3f(x2, y1, -z1);
	glVertex3f(x2, y2, -z1);
	glVertex3f(x1, y2, -z1);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3f(x1, y1, -z2);
	glVertex3f(x2, y1, -z2);
	glVertex3f(x2, y2, -z2);
	glVertex3f(x1, y2, -z2);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(x1, y1, -z1);
	glVertex3f(x1, y1, -z2);
	glVertex3f(x1, y2, -z1);
	glVertex3f(x1, y2, -z2);
	glVertex3f(x2, y1, -z1);
	glVertex3f(x2, y1, -z2);
	glVertex3f(x2, y2, -z1);
	glVertex3f(x2, y2, -z2);
	glEnd();
}

void draw_quad (GLfloat x1, GLfloat y1, GLfloat w, GLfloat h, GLfloat z)
{
	glBegin(GL_QUADS);
	glVertex3f(x1,   y1,   z);
	glVertex3f(x1+w, y1,   z);
	glVertex3f(x1+w, y1+h, z);
	glVertex3f(x1,   y1+h, z);
	glEnd();
}

/* calculate texture coordinates for the hatches */

void tx_ll(int hatch)
{
	int x, y;
	GLfloat tx, ty;

	x = (hatch - 1) % 4, y = (hatch - 1) / 4;
	tx = 0.25 * x, ty = 40.0/256.0 * y;
	glTexCoord2f(tx, ty);
}

void tx_lr(int hatch)
{
	int x, y;
	GLfloat tx, ty;

	x = (hatch - 1) % 4, y = (hatch - 1) / 4;
	tx = 0.25 * (x + 1), ty = 40.0/256.0 * y;
	glTexCoord2f(tx, ty);
}

void tx_ur(int hatch)
{
	int x, y;
	GLfloat tx, ty;

	x = (hatch - 1) % 4, y = (hatch - 1) / 4;
	tx = 0.25 * (x + 1), ty = 40.0/256.0 * (y + 1);
	glTexCoord2f(tx, ty);
}

void tx_ul(int hatch)
{
	int x, y;
	GLfloat tx, ty;

	x = (hatch - 1) % 4, y = (hatch - 1) / 4;
	tx = 0.25 * x, ty = 40.0/256.0 * (y + 1);
	glTexCoord2f(tx, ty);
}
