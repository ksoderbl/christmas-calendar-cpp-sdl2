#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

#include <GL/gl.h>
#include <GL/glu.h>

#include <SDL2/SDL.h>

#include "effect.h"
#include "messages.h"
#include "pause.h"

#define DATADIR "../data"

/*#include "texture.h"*/
/*#include "picking.h"*/
/*#include "vectormath.h"*/

/* hack ... we have a do { } while (dummy) below,
 * if I change it to  do { } while (0) irix cc says
 * "The controlling expression is constant."
 */
extern int dummy;

/* ---------------------------------------------------------------------- */

/*
 * SGI IRIX cc likes to complain about unused variables if you compile with
 * -fullwarn
 */


/* constants */
#define NUMEFFECTS  25
#define ASCII_ESC   27
#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#define CALENDAR_DELAY 1 /* seconds of delay before running a chosen effect */
/* flash parameters */
#define FLASH_INTENSITY_MAX 1.5
#define FLASH_INTENSITY_MIN 0.5
#define FLASH_INTENSITY_DEC 0.05



/* ---------------------------------------------------------------------- */

extern void main_swap_window(void);
extern void main_run_effect(int);
#define return_to_calendar() main_run_effect(0)

/* for use by pausing functions etc */
void main_set_callbacks(struct effect *, int w, int h);
extern struct effect *current_effect;

extern void mainKeyboardFunc(effect_keyboard_func func);
extern void mainMouseFunc(effect_mouse_func func);
extern void mainSwapBuffers(void);
extern void mainPostRedisplay(void);

/* these return 0 if ok, -1 if stub */
extern int effect0_register(struct effect *); /* calendar */
extern int effect1_register(struct effect *);
extern int effect2_register(struct effect *);
extern int effect3_register(struct effect *);
extern int effect4_register(struct effect *);
extern int effect5_register(struct effect *);
extern int effect6_register(struct effect *);
extern int effect7_register(struct effect *);
extern int effect8_register(struct effect *);
extern int effect9_register(struct effect *);
extern int effect10_register(struct effect *);
extern int effect11_register(struct effect *);
extern int effect12_register(struct effect *);
extern int effect13_register(struct effect *);
extern int effect14_register(struct effect *);
extern int effect15_register(struct effect *);
extern int effect16_register(struct effect *);
extern int effect17_register(struct effect *);
extern int effect18_register(struct effect *);
extern int effect19_register(struct effect *);
extern int effect20_register(struct effect *);
extern int effect21_register(struct effect *);
extern int effect22_register(struct effect *);
extern int effect23_register(struct effect *);
extern int effect24_register(struct effect *);

#endif /* MAIN_H */
