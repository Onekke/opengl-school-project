#pragma once
#include "pgr.h"
#include "data.h"
#include "model.h"

class Object
{
	// object parameters
	float	  angle;
	float     scale;
	float	  speed;
	float	  size;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 direction;

	// view and projection matrixes
	glm::mat4 view;
	glm::mat4 projection;

	// current time of an object time
	float currentTime;

public:
	Model* model;

	//Object(const std::string& fileName, Shader* shader);
	Object(Model* model);
	~Object() {};

	void ReloadModel(Model* model);

	glm::vec3 GetPosition();
	glm::vec3 GetDirection();
	float GetSpeed();
	float GetSize();
	float GetAngle();

	void Update(float elapsedTime, bool* keyboard, bool passive);
	void SetDirection(float angle);

	void Reset(glm::vec3 position, glm::vec3 rotation, float scale, float angle, float size);
	void Draw(const glm::mat4& view, const glm::mat4& projection, glm::vec3 cameraPosition, Object* cameraTarget);
	void SetRandomPosition(Object* object);
	bool PointInSphere(const glm::vec3& point, const glm::vec3& center, float radius);
};