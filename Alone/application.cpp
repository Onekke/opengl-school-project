#include "application.h"
void Application::UpdateWindow(int width, int height) {
	windowWidth  = width;
	windowHeight = height;
}

void Application::UpdateTime() {
	elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME);
}

void Application::Reset() {
	firstMouse = true;

	printf("---\tkeyboard keymap\n");
	// reset keymap
	for (int i = 0; i < KEYS_COUNT; i++) {
		keyboard[i] = false;
	}

	printf("---\tmouse buttons map\n");
	// reset mouse
	for (int i = 0; i < BUTTONS_COUNT; i++) {
		mouse[i] = false;
	}
}

bool Application::CheckCollisions(Object* a, Object* b) {
	glm::vec3 center1 = a->GetPosition();
	glm::vec3 center2 = b->GetPosition();
	float radius1	  = a->GetSize();
	float radius2	  = b->GetSize();
	return (glm::dot(center1 - center2, center1 - center2) <= (radius1 + radius2) * (radius1 + radius2))?true:false;
}