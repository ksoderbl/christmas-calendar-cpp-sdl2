#ifndef EFFECT_H
#define EFFECT_H

#include <iostream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

#include <SDL2/SDL.h>

class Effect {
public:
	virtual int init() = 0;
	virtual void cleanup() = 0;
	void reset();
	
	string name();

};

class Effect0 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect1 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect2 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect3 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect4 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect5 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect6 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect7 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect8 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect9 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect10 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect11 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect12 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect13 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect14 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect15 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect16 : public Effect {
public:
	int init();
	void cleanup();
	
};

class Effect17 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect18 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect19 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect20 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect21 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect22 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect23 : public Effect {
public:
	int init();
	void cleanup();
};

class Effect24 : public Effect {
public:
	int init();
	void cleanup();
};

struct effect;

/* effect "methods" */

typedef int  (*effect_registration_func)(struct effect *);

typedef void (*effect_display_func)(void);
typedef void (*effect_reshape_func)(int width, int height);
typedef void (*effect_keyboard_func)(SDL_KeyboardEvent key);
typedef void (*effect_mouse_func)(SDL_MouseButtonEvent button);
typedef void (*effect_motion_func)(SDL_MouseMotionEvent motion);
typedef void (*effect_idle_func)(void);


typedef struct effect {
	effect_registration_func  e_register;
	
	effect_display_func       e_display;
	effect_reshape_func       e_reshape;
	effect_keyboard_func      e_keyboard;
	effect_mouse_func         e_mouse;
	effect_motion_func        e_motion;
	effect_idle_func          e_idle;

	string                    e_name;
} effect_t;




/* ---------------------------------------------------------------------- */

#endif
