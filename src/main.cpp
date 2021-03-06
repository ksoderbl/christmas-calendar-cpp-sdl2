#include <iostream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

#include <GL/gl.h>
#include <GL/glu.h>

#include <SDL2/SDL.h>

#include <cassert>
#include <cmath>
#include <ctime>
#include <csignal>
#include <cstdarg>
#include <cstdlib>

#include "main.h"
#include "messages.h"
#include "stars.h"
#include "quit.h"

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


/* ---------------------------------------------------------------------- */

static void main_idle_cb(void);
static void main_flash(void);

/* ---------------------------------------------------------------------- */

static Effect *effect_objects[NUMEFFECTS] = {0, };
int currentEffectIndex = -1;
static Effect *currentEffect = 0;
bool pausing = false;

int init_effects(void)
{
	int ei;
	
	effect_objects[0] = new CalendarEffect();
	effect_objects[1] = new Effect1();
	effect_objects[2] = new Effect2();
	effect_objects[3] = new Effect3();
	effect_objects[4] = new Effect4();
	effect_objects[5] = new Effect5();
	effect_objects[6] = new Effect6();
	effect_objects[7] = new Effect7();
	effect_objects[8] = new Effect8();
	effect_objects[9] = new Effect9();
	effect_objects[10] = new Effect10();
	effect_objects[11] = new Effect11();
	effect_objects[12] = new Effect12();
	effect_objects[13] = new Effect13();
	effect_objects[14] = new Effect14();
	effect_objects[15] = new Effect15();
	effect_objects[16] = new Effect16();
	effect_objects[17] = new Effect17();
	effect_objects[18] = new Effect18();
	effect_objects[19] = new Effect19();
	effect_objects[20] = new Effect20();
	effect_objects[21] = new Effect21();
	effect_objects[22] = new Effect22();
	effect_objects[23] = new Effect23();
	effect_objects[24] = new Effect24();

	for (ei = 0; ei < NUMEFFECTS; ei++) {
		cout << "effect" << ei << "name: " << effect_objects[ei]->getName() << "\n";
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

int init_sdl(void)
{
	int val;
	
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
		//exit(1);
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
 */
void main_run_effect(int ei)
{
	int err, old;
	GLint vp[4];

	/* deallocate previous effect */
	if (next_effect == -1)
		next_effect = 0;
	else {
		old = next_effect;
		effect_objects[old]->cleanup();
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
	err = effect_objects[ei]->init();
	if (err < 0) {
		cerr << "effect " << ei
		     << " [\"" << effect_objects[ei]->getName() << "\"] failed to initialize" << endl;
		exit(1);
	}

	glGetIntegerv(GL_VIEWPORT, vp);

	effect_objects[ei]->resize((int)vp[2], (int)vp[3]);

	next_effect = ei;
	
	main_flash();
}

void main_set_current_effect_index(int ei)
{
	if (ei < 0) {
		cerr << "main_set_current_effect_index: ei < 0\n";
		exit(1);
	}
	if (ei >= NUMEFFECTS ) {
		cerr << "main_set_current_effect_index: ei >= NUMEFFECTS\n";
		exit(1);
	}

	currentEffectIndex = ei;
	currentEffect = effect_objects[currentEffectIndex];
	
	displayFunc = 0;
	reshapeFunc = 0;
	keyboardFunc = 0;
	mouseFunc = 0;
	motionFunc = 0;
	idleFunc = 0;
}

/* 
 * After main_run_effect() and a flash() we can actually
 * call the main function of the next effect. It must
 * set all the appropriate main callbacks.
 */
static void run_next_effect(void)
{
	int ei;

	ei = next_effect;

	main_set_current_effect_index(ei);
}








/* ---------------------------------------------------------------------- */


static void flash_display_cb(void);
/* clear color */
static GLdouble cc;

static void main_flash(void)
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
}

/* ---------------------------------------------------------------------- */

static int main_run = 0;

static void main_idle_cb(void)
{

}

/* ---------------------------------------------------------------------- */

static void handle_keydown(SDL_KeyboardEvent key)
{
	cout << "KEYDOWN, current effect = " << currentEffectIndex << endl;

	if (pausing) {
		cout << "pause keyboard cb\n";

		switch (key.keysym.sym) {
			/* quit pausing */
		case SDLK_SPACE:
		case SDLK_p:
		case SDLK_PAUSE:
		case SDLK_ESCAPE:
			pausing = false;
			break;
		default:
			break;
		}

		return;
	}


	// not pausing
	
	switch (key.keysym.sym) {
	case SDLK_ESCAPE:
		if (currentEffectIndex == 0) {
			quit_request();
		}
		else if (currentEffectIndex > 0) {
			main_run_effect(0); //return_to_calendar();
		}
		break;
	case SDLK_m:
		messages_toggle();
		break;

	case SDLK_p:
	case SDLK_PAUSE:
		// TODO: pause_request();
		break;
	case SDLK_r:
		if (currentEffect)
			currentEffect->reset();
		break;

	default:
		if (currentEffect)
			currentEffect->keyboardEvent(key);
	}
}


void main_loop(void)
{
	time_t oldt = 0, t = 0;
	bool loop = true;

	while (loop)
	{
		t = time(0);
		if (t != oldt) {
			//cout << "time is " << t << endl;
			oldt = t;
		}
		
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				loop = false;

			else if (event.type == SDL_KEYDOWN)
			{
				handle_keydown(event.key);
			}

			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				cout << "mouse button down, current effect is " << currentEffectIndex << endl;
				if (currentEffect)
					currentEffect->mouseButtonDownEvent(event.button);
			}

			else if (event.type == SDL_MOUSEMOTION)
			{
				if (event.motion.state & SDL_BUTTON_LMASK) {
					if (currentEffect)
						currentEffect->mouseMotionEvent(event.motion);
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
					
					if (currentEffect)
						currentEffect->resize(w, h);
					break;
				default:
					break;
				}
			}
		}

                if (idleFunc == main_idle_cb) {
			//cout << "idle func is main idle cb" << endl;
			if (!main_run) {
				cout << "CALLING MAIN_RUN_EFFECT(0)" << endl;
				main_run_effect(0);
				main_run = 1;
			}
		} else if (idleFunc == calendar_idle_cb) {
			//cout << "idle func is calendar idle cb" << endl;
			calendar_idle_cb();
		} else if (idleFunc == 0) {
			//cout << "idle func is 0" << endl;
			calendar_idle_cb();
		} else {
			cout << "idle func is " << idleFunc << endl;
			assert(false);
		}
		
		if (displayFunc)
			(*displayFunc)();
		else if (currentEffect)
			currentEffect->drawFrame();
		
		SDL_GL_SwapWindow(window);
	} // while loop
}


int main(int argc, char *argv[])
{
	init_effects();

	srand(time(NULL));
	
	init_sdl();

	/* store default attributes (redbook 2nd ed. p. 79) */
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	idleFunc = main_idle_cb;
	
	main_loop();
	
	cleanup_sdl();

	return 0;
}
