#ifndef CALENDAR_PRIMITIVES_H
#define CALENDAR_PRIMITIVES_H

#include "main.h"


extern void draw_triangle (GLfloat x1, GLfloat y1, 
			   GLfloat x2, GLfloat y2,
			   GLfloat x3, GLfloat y3,  GLfloat z);

extern void draw_view_volume (GLfloat x1, GLfloat x2,
			      GLfloat y1, GLfloat y2, 
			      GLfloat z1, GLfloat z2);

extern void draw_quad (GLfloat x1, GLfloat y1,
		       GLfloat w, GLfloat h, GLfloat z);

extern void tx_ll(int hatch);
extern void tx_lr(int hatch);
extern void tx_ur(int hatch);
extern void tx_ul(int hatch);


#endif /* CALENDAR_PRIMITIVES_H */

