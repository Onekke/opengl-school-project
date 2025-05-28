#pragma once
#include "pgr.h"
#include "data.h"
#include "object.h"

enum CAMERA_MODE { FIRST_PERSON, THIRD_PERSON };

// default camera parameters
const glm::vec3 POSITION = glm::vec3(0.0f, 0.5f, 1.0f);
const glm::vec3 TARGET	 = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 UP		 = glm::vec3(0.0f, 1.0f, 0.0f);
const float		YAW		 = -90.0f;
const float		PITCH	 = 30.0f;
const int		MODE	 = THIRD_PERSON;
const bool		LOCKED	 = TRUE;
const float		FOV		 = 60.0f;
const float		ZOOM	 = 0.5f;

// max values
const float		ELEVATION_MAX = 45.0f;

class Camera {

	// euler angles
	float yaw;
	float pitch;

	// camera view mode
	int mode;

	// camera lock (free movement)
	bool lock;

	// camera perspective
	float fov;

	// camera zoom
	float zoom;

public:
	// camera's target
	Object* object;

	// camera vectors
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 up;
	glm::vec3 direction;

	// camera view matrix
	glm::mat4 view;

	// camera projection matrix;
	glm::mat4 projection;

	Camera(Object* object);
	~Camera() {};

	bool Lock();
	bool Locked();
	bool FirstPerson();
	void Reset();
	void SwitchMode();
	void Update(float width, float height);
	void Rotate(float x, float y);
	void Zoom(float dir);

private:
	void UpdateVectors();
	void UpdateView();
	void UpdateProjection();
	void UpdateProjection(float width, float height);
};