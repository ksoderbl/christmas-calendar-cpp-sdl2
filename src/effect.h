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
	virtual void drawFrame() = 0;
	virtual void resize(int width, int height) = 0;
	virtual void reset() = 0;
	virtual string getName() = 0;
	virtual void keyboardEvent(SDL_KeyboardEvent key) = 0;
	virtual void mouseButtonDownEvent(SDL_MouseButtonEvent button) = 0;
	virtual void mouseMotionEvent(SDL_MouseMotionEvent motion) = 0;
};

class CalendarEffect : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect1 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect2 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect3 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect4 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect5 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect6 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect7 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect8 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect9 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect10 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect11 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect12 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect13 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect14 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect15 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect16 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect17 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect18 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect19 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect20 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect21 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect22 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect23 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

class Effect24 : public Effect {
public:
	int init();
	void cleanup();
	void drawFrame();
	void resize(int width, int height);
	void reset();
	string getName();
	void keyboardEvent(SDL_KeyboardEvent key);
	void mouseButtonDownEvent(SDL_MouseButtonEvent button);
	void mouseMotionEvent(SDL_MouseMotionEvent motion);
};

typedef void (*effect_display_func)(void);
typedef void (*effect_reshape_func)(int width, int height);
typedef void (*effect_keyboard_func)(SDL_KeyboardEvent key);
typedef void (*effect_mouse_func)(SDL_MouseButtonEvent button);
typedef void (*effect_motion_func)(SDL_MouseMotionEvent motion);
typedef void (*effect_idle_func)(void);

#endif
