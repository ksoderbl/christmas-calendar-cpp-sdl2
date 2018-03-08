#ifndef PICKING_H
#define PICKING_H

#include <GL/gl.h>

/* returns integer name for object hit or -1 if no hit */

int pick_object(int x, int y, void (*draw_scene)(GLenum mode));

#endif
