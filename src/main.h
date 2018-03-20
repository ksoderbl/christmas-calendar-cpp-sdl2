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

/* ---------------------------------------------------------------------- */

/* constants */
#define NUMEFFECTS  25
#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#define CALENDAR_DELAY 1 /* seconds of delay before running a chosen effect */
/* flash parameters */
#define FLASH_INTENSITY_MAX 1.5
#define FLASH_INTENSITY_MIN 0.5
#define FLASH_INTENSITY_DEC 0.05

/* ---------------------------------------------------------------------- */

extern void calendar_idle_cb(void);
extern void main_swap_window(void);
extern void main_run_effect(int);

/* for use by pausing functions etc */
void main_set_callbacks(struct effect *ep, int w, int h);

extern bool pausing;

extern void mainKeyboardFunc(effect_keyboard_func func);
extern void mainMouseFunc(effect_mouse_func func);

#endif /* MAIN_H */
