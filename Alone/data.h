#pragma once
#include "pgr.h"

// OpenGL window
#define WINDOW_WIDTH		800
#define WINDOW_HEIGHT		600
#define WINDOW_TITLE		"Alone"

#define REFRESH_INTERVAL	33

enum { KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_SPACE, KEYS_COUNT };
enum { RIGHT_MOUSE_BUTTON, MIDDLE_MOUSE_BUTTON, LEFT_MOUSE_BUTTON, BUTTONS_COUNT };

// scene
void renderScene(void);
void updateScene(void);

// application
bool init(void);
void restart(void);

// callbacks
void displayCb(void);
void reshapeCb(int width, int height);
void timerCb(int id);
void passiveMotionCb(int x, int y);
void motionCb(int x, int y);
void mouseCb(int button, int state, int x, int y);
void mouseWheelCb(int wheel, int dir, int x, int y);
void keyboardCb(unsigned char key, int x, int y);
void keyboardUpCb(unsigned char key, int x, int y);
void specialKeyboardCb(int key, int x, int y);
void specialKeyboardUpCb(int key, int x, int y);