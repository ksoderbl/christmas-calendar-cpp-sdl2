#include <iostream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

#include <GL/gl.h>
#include <GL/glu.h>

#include <SDL2/SDL.h>

#include <math.h>
#include <time.h>
#include <signal.h>
#include <stdarg.h>

#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "messages.h"
#include "stars.h"

SDL_Window *window;
SDL_GLContext glContext;

static effect_display_func       displayFunc = 0;
static effect_reshape_func       reshapeFunc = 0;
static effect_keyboard_func      keyboardFunc = 0;
static effect_mouse_func         mouseFunc = 0;
static effect_motion_func        motionFunc = 0;
static effect_idle_func          idleFunc = 0;

void mainKeyboardFunc(effect_keyboard_func func)
{
	keyboardFunc = func;
}
void mainMouseFunc(effect_mouse_func func)
{
	mouseFunc = func;
}
void mainSwapBuffers(void)
{
	SDL_GL_SwapWindow(window);
}
void mainPostRedisplay(void)
{
	; //SDL_GL_SwapWindow(window);
}


/* ---------------------------------------------------------------------- */


static void main_reshape_cb(int w, int h);
static void main_display_cb(void);
static void main_idle_cb(void);
static void darkness(void);
static void flash(void);

int dummy=0; /* don't change, see main.h */

/* ---------------------------------------------------------------------- */

static struct effect *effects = NULL;
struct effect *current_effect = NULL;

int init_effects(void)
{
	int err = 0, ei;
	
	effects = (effect_t *) calloc(NUMEFFECTS, sizeof(struct effect));
	if (!effects) {
		cerr << "calloc failed" << endl;
		exit(1);
	}

	effects[0].e_register  = effect0_register;
	effects[1].e_register  = effect1_register;
	effects[2].e_register  = effect2_register;
	effects[3].e_register  = effect3_register;
	effects[4].e_register  = effect4_register;
	effects[5].e_register  = effect5_register;
	effects[6].e_register  = effect6_register;
	effects[7].e_register  = effect7_register;
	effects[8].e_register  = effect8_register;
	effects[9].e_register  = effect9_register;
	effects[10].e_register = effect10_register;
	effects[11].e_register = effect11_register;
	effects[12].e_register = effect12_register;
	effects[13].e_register = effect13_register;
	effects[14].e_register = effect14_register;
	effects[15].e_register = effect15_register;
	effects[16].e_register = effect16_register;
	effects[17].e_register = effect17_register;
	effects[18].e_register = effect18_register;
	effects[19].e_register = effect19_register;
	effects[20].e_register = effect20_register;
	effects[21].e_register = effect21_register;
	effects[22].e_register = effect22_register;
	effects[23].e_register = effect23_register;
	effects[24].e_register = effect24_register;

	/*
	 * call all effect registration functions to get function pointers
	 * to their init, main and cleanup functions
	 */
	for (ei = 0; ei < NUMEFFECTS; ei++) {
		err = (*effects[ei].e_register)(&effects[ei]);
		if (err < 0) {
			cerr << "registration of effect " << ei << " is a stub" << endl;
		} else {
			cout << "effect" << ei << "name         : " << effects[ei].e_name << "\n";
		}
	}

	stars_make();

	return 0;
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */

/* some initial values for the application */

const int S_WIDTH = 800;
const int S_HEIGHT = 600;
// e.g. SDL_WINDOWPOS_CENTERED, or SDL_WINDOWPOS_UNDEFINED
const int S_XPOS = SDL_WINDOWPOS_UNDEFINED;
const int S_YPOS = SDL_WINDOWPOS_UNDEFINED;
const string S_APPNAME = "Christmas Calendar in C++ and SDL2";

const int WIDTH = S_WIDTH;
const int HEIGHT = S_HEIGHT;
const int FPS = 60;
	
// number of pixels in texture
const int TWIDTH = 64;
const int THEIGHT = 16;
const int TBPP = 3;  // bytes per pixel
const bool SMOOTH_PIXELS=true;

int init_sdl(void)
{
	int val;
	
	//mainInit(&argc, argv);
	//mainInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//mainInitWindowSize(S_WIDTH, S_HEIGHT);
	//mainInitWindowPosition(S_XPOS, S_YPOS);
	//mainCreateWindow(S_APPNAME);
	
	//if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "SDL_Init failed: " << SDL_GetError() << endl;
		exit(1);
	}
    
	Uint32 flags = SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;
    
	window = SDL_CreateWindow(S_APPNAME.c_str(), S_XPOS, S_YPOS, S_WIDTH, S_HEIGHT, flags);
	if (!window) {
		cerr << "Create window failed: " << SDL_GetError() << endl;
		exit(1);
	}
	
	glContext = SDL_GL_CreateContext(window);
	if (!glContext) {
		cerr << "Create GL context failed: " << SDL_GetError() << endl;
		exit(1);
	}
	
	// needed ???
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	
	val = SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if (val < 0) {
		cerr << "Double buffering failed: " << SDL_GetError() << endl;
		exit(1);
	}
	
	val = SDL_GL_SetSwapInterval(1);
	if (val < 0) {
		cerr << "Synchronizing with vertical retrace failed: " << SDL_GetError() << endl;
		exit(1);
	}
	
	return 0;
}

/* ---------------------------------------------------------------------- */

void cleanup_sdl(void)
{
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

/* ---------------------------------------------------------------------- */



static int next_effect = -1;

/*
 * Function main_run_effect should be called from an effect to
 * relinquish control. If used from the calendar it will direct
 * the main code to start some other effect.
 * The other effects should use main_run_effect(0) or
 * return_to_calendar().
 */
void main_run_effect(int ei)
{
	int err, old;
	GLint vp[4];

	if (*effects[ei].e_init == NULL) {
		cerr << "effect " << ei
		     << " [\"" << effects[ei].e_name << "\"] has no init method" << endl;
		return;
	}

	darkness();
	mainPostRedisplay();
	
	/* deallocate previous effect */
	if (next_effect == -1)
		next_effect = 0;
	else {
		old = next_effect;
		(*effects[old].e_cleanup)(&effects[old]);
	}

	/* Restore default attributes.
	 * (check red book 2nd ed. p. 79)
	 * We do not want to restore the viewport size if
	 * it has changed.
	 */
	glGetIntegerv(GL_VIEWPORT, vp);
	glPopAttrib();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glViewport(vp[0], vp[1], vp[2], vp[3]);

	/* init frame count */
	messages_init_fps();

	/* allocate memory, do precomputations etc. */
	/* note: if the effect is not initialized correctly,
	 * we might get e.g. a segmentation fault here */
	err = (*effects[ei].e_init)(&effects[ei]);
	if (err < 0) {
		cerr << "effect " << ei
		     << " [\"" << effects[ei].e_name << "\"] failed to initialize" << endl;
		exit(1);
	}

	glGetIntegerv(GL_VIEWPORT, vp);
	(*effects[ei].e_reshape)((int)vp[2], (int)vp[3]);

	next_effect = ei;
	
	flash();
}

void main_set_callbacks(struct effect *ep, int w, int h)
{
	if (!ep) {
		cerr << "main_set_callbacks: ep NULL\n";
		exit(1);
	}

	displayFunc = ep->e_display;
	reshapeFunc = ep->e_reshape;
	keyboardFunc = ep->e_keyboard;
	mouseFunc = ep->e_mouse;
	motionFunc = ep->e_motion;
	idleFunc = ep->e_idle;
	
	/* if we resized during pause */
	if (w != -1) {
		(ep->e_reshape)(w, h);
	}
	mainPostRedisplay();
}

/* 
 * After main_run_effect() and a flash() we can actually
 * call the main function of the next effect. It must
 * set all the appropriate main callbacks.
 */
static void run_next_effect(void)
{
	int ei;
	struct effect *ep;

	ei = next_effect;
	ep = &effects[ei];

	/* set the effect as the current effect */
	current_effect = ep;

	/* set the callback, activating the new effect */
	main_set_callbacks(ep, -1, -1);

	/* make sure we display the stuff */
	mainPostRedisplay();
}








/* ---------------------------------------------------------------------- */


static void flash_display_cb(void);
/* clear color */
static GLdouble cc;

static void flash(void)
{
	displayFunc = flash_display_cb;
	idleFunc = main_idle_cb;
}

static int flash_init_ok = 0;
static GLfloat ccc[4];


static void flash_display_cb(void)
{
	/* let's not trash the clear color set by the effect */


	if (!flash_init_ok) {
		glGetFloatv(GL_COLOR_CLEAR_VALUE, ccc);

		cc = FLASH_INTENSITY_MAX;
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0.0, 0.0, 5.0,  /* eye */
			  0.0, 0.0, 0.0,  /* ctr */
			  0.0, 1.0, 0.0); /* up */
		flash_init_ok = 1;
	}

	glClearColor(cc, cc, cc, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cc -= FLASH_INTENSITY_DEC;

	if (cc < FLASH_INTENSITY_MIN) {
		flash_init_ok = 0;
		glClearColor(ccc[0], ccc[1], ccc[2], ccc[3]);
		run_next_effect();
	}

	SDL_GL_SwapWindow(window);
}


/* ---------------------------------------------------------------------- */


static void main_reshape_cb(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w/(GLdouble)h, 0.1, 100.0);
}

static void main_display_cb(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SDL_GL_SwapWindow(window);
}

static int main_run = 0;

static void main_idle_cb(void)
{
	if (!main_run) {
		main_run_effect(0);
		main_run = 1;
	}
	mainPostRedisplay();
}


static void darkness(void)
{
	displayFunc = main_display_cb;
	reshapeFunc = main_reshape_cb;
	idleFunc = main_idle_cb;
	mouseFunc = 0;
	keyboardFunc = 0;
}

/* ---------------------------------------------------------------------- */

void main_loop(void)
{
	bool loop = true;
	//GLubyte texture[THEIGHT][TWIDTH][3];
	//GLubyte *texture;
	//GLuint texname = 0;
	
	//texture = (GLubyte *) malloc ( THEIGHT * TWIDTH * 3 );
	// assume allocation worked

	//glClearColor(0.0, 0.0, 0.5, 0.0);
	//glColor3f(1.0, 1.0, 1.0);
	//glOrtho(0.0, WIDTH, 0.0, HEIGHT, -1.0, 1.0);
	//int j = 0;

	while (loop)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				loop = false;

			else if (event.type == SDL_KEYDOWN)
			{
				if (keyboardFunc)
					(*keyboardFunc)(event.key /*.keysym.sym*/);
				
				/*
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					loop = false;
					break;
				case SDLK_r:
					// Cover with red and update
					glClearColor(1.0, 0.0, 0.0, 1.0);
					glClear(GL_COLOR_BUFFER_BIT);
					SDL_GL_SwapWindow(window);
					break;
				case SDLK_g:
					// Cover with green and update
					glClearColor(0.0, 1.0, 0.0, 1.0);
					glClear(GL_COLOR_BUFFER_BIT);
					SDL_GL_SwapWindow(window);
					break;
				case SDLK_b:
					// Cover with blue and update
					glClearColor(0.0, 0.0, 1.0, 1.0);
					glClear(GL_COLOR_BUFFER_BIT);
					SDL_GL_SwapWindow(window);
					break;
				default:
					break;
				}
				*/
			}

			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (mouseFunc)
					(*mouseFunc)(event.button);
			}

			else if (event.type == SDL_MOUSEMOTION)
			{
				if (event.motion.state & SDL_BUTTON_LMASK) {
					if (motionFunc)
						(*motionFunc)(event.motion);
				}
			}
			
			else if (event.type == SDL_WINDOWEVENT)
			{
				int w, h;
				switch (event.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				case SDL_WINDOWEVENT_RESIZED:
					w = event.window.data1;
					h = event.window.data2;
					
					if (reshapeFunc)
						(*reshapeFunc)(w, h);
					break;
				default:
					break;
				}
			}
		}
	     
		if (idleFunc)
			(*idleFunc)();
		
		if (displayFunc)
			(*displayFunc)();
		
		SDL_GL_SwapWindow(window);
	} // while loop
}


int main(int argc, char *argv[])
{
	init_effects();
	srand(time(NULL));
	dummy = 0;
	
	init_sdl();

	/* store default attributes (redbook 2nd ed. p. 79) */
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	reshapeFunc = main_reshape_cb;
	displayFunc = main_display_cb;
	idleFunc = main_idle_cb;
	
	main_loop();
	
	cleanup_sdl();

	return 0;
}
