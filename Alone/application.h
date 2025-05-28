#pragma once
#include "pgr.h"
#include "data.h"
#include "object.h"
class Application
{
public:
	// updated window size after reshapeCb()
	int windowWidth;
	int windowHeight;

	// camera view matrix
	glm::mat4 view;

	// camera projection matrix;
	glm::mat4 projection;

	// keyboard keymap
	bool  keyboard[KEYS_COUNT];

	// mouse buttons map
	bool  mouse[BUTTONS_COUNT];

	// cursor position
	bool firstMouse;
	int lastX;
	int lastY;

	float elapsedTime;

	Application() {};
	~Application() {};

	void UpdateWindow(int width, int height);
	void UpdateTime();
	void Reset();
	bool CheckCollisions(Object* a, Object* b);
};