#include <iostream>
using std::cerr;
#include <math.h>
#include <GL/gl.h>
#include "main.h"

/* calculate Euclidean distance between 3D points
 * v1 and v2
 */
double
distance(double v1[3], double v2[3])
{
	int i;
	double d[3];
	
	for (i = 0; i < 3; i++)
		d[i] = v1[i] - v2[i];
	return sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);
}				


double
dotprod(double a[3], double b[3])
{
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}


/* calculates n = v1 x v2 */
void
normcrossprod3(double n[3], double a[3], double b[3])
{
	int i;
	double len;

	n[0] = a[1] * b[2] - a[2] * b[1];
	n[1] = a[2] * b[0] - a[0] * b[2];
	n[2] = a[0] * b[1] - a[1] * b[0];

	len = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
	
	if (len == 0.0) {
		cerr << "normal length is 0\n";
		return;
	}

	for (i = 0; i < 3; i++)
		n[i] /= len;
}				

void
normcrossprod3f(GLfloat n[3], GLfloat a[3], GLfloat b[3])
{
	int i;
	GLfloat len;

	n[0] = a[1] * b[2] - a[2] * b[1];
	n[1] = a[2] * b[0] - a[0] * b[2];
	n[2] = a[0] * b[1] - a[1] * b[0];

	len = (GLfloat)sqrt((double)(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]));
	
	if (len == 0.0) {
		cerr << "normal length is 0\n";
		return;
	}

	for (i = 0; i < 3; i++)
		n[i] /= len;
}				

/* calculate the n = (v1 - v0) x (v2 - v0) */
void
normcrossprod4(double n[3], double v0[3],
	       double v1[3], double v2[3])
{
	int i;
	double v3[3], v4[3];

	for (i = 0; i < 3; i++) {
		v3[i] = v1[i] - v0[i];
		v4[i] = v2[i] - v0[i];
	}

	normcrossprod3(n, v3, v4);
}

/* calculate the n = (v1 - v0) x (v2 - v0) */
void
normcrossprod4f(GLfloat n[3], GLfloat v0[3],
	       GLfloat v1[3], GLfloat v2[3])
{
	int i;
	GLfloat v3[3], v4[3];

	for (i = 0; i < 3; i++) {
		v3[i] = v1[i] - v0[i];
		v4[i] = v2[i] - v0[i];
	}

	normcrossprod3f(n, v3, v4);
}
