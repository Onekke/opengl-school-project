#include "camera.h"

Camera::Camera(Object* object) {
	// set cameras view target
	Camera::object = object;
	mode = THIRD_PERSON;
	lock = true;
	up   = UP;
	fov  = FOV;
	zoom = ZOOM;
}

bool Camera::Lock() {
	lock = !lock;
	Reset();
	return lock;
}

bool Camera::Locked() {
	return lock;
}

bool Camera::FirstPerson() {
	return (mode == FIRST_PERSON) ? true : false;
}

void Camera::Reset() {
	yaw = object->GetAngle();
	if (mode == FIRST_PERSON) {
		pitch = 0.0f;
		zoom = ZOOM;
	} else {
		pitch = 30.0f;
	}

	//fov  = FOV;
	//zoom = ZOOM;

	// calculate camera vectors
	UpdateVectors();

	// calculate view matrix
	UpdateView();

	// calculate projection matrix
	UpdateProjection();
}

void Camera::SwitchMode() {
	if (mode == FIRST_PERSON) {
		printf("--- camera mode: THIRD PERSON ---\n");
		mode = THIRD_PERSON;
	}
	else {
		printf("--- camera mode: FIRST PERSON ---\n");
		mode = FIRST_PERSON;
	}

	Reset();
}

void Camera::Update(float width, float height) {
	UpdateVectors();
	UpdateView();
	UpdateProjection(width, height);
}

void Camera::Rotate(float x, float y) {
	yaw -= x * 0.5f;
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}
	else if (yaw < 360.0f) {
		yaw += 360.0f;
	}
	pitch += y * 0.5f;

	if (pitch > ELEVATION_MAX)
		pitch = ELEVATION_MAX;
	if (pitch < -ELEVATION_MAX)
		pitch = -ELEVATION_MAX;

	if (mode == FIRST_PERSON)
		object->SetDirection(yaw);
}

void Camera::Zoom(float dir) {
	zoom += dir*0.01f;
	if (zoom < 0.1f)
		zoom = 0.1f;
	if (zoom > 2.0f)
		zoom = 2.0f;
}

void Camera::UpdateView() {
	view = glm::lookAt(position, target, up);
}

void Camera::UpdateProjection() {
	projection = glm::perspective(glm::radians(fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.05f, 100.0f);
}

void Camera::UpdateProjection(float width, float height) {
	projection = glm::perspective(glm::radians(fov), width / height, 0.05f, 100.0f);
}

void Camera::UpdateVectors() {
	if (mode == THIRD_PERSON) {
		target = object->GetPosition() + glm::vec3(0.0f, 0.1f, 0.0f);
		if (lock)
			direction = -object->GetDirection() + glm::vec3(0.0f, sin(glm::radians(pitch)), 0.0f);
		else
			direction = glm::vec3(-sin(glm::radians(yaw)), sin(glm::radians(pitch)), -cos(glm::radians(yaw)));
		position	  = target + glm::normalize(direction)*zoom;

	} else {
		if (lock)
			yaw	  = object->GetAngle();
		direction = glm::normalize(glm::vec3(cos(glm::radians(pitch)) * sin(glm::radians(yaw)),
											 sin(glm::radians(pitch)),
											 cos(glm::radians(pitch)) * cos(glm::radians(yaw))));
		position = object->GetPosition() + glm::vec3(0.0f, 0.15f, 0.0f);

		target = direction + position;
	}
}