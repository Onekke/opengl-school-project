#pragma once
#include "pgr.h"
#include "explosion_shader.h"

class Explosion
{
	ExplosionShader* shader;

	// explosion parameters
	glm::vec3 position;
	glm::vec3 direction;
	float scale;

public:
	// time 
	float currentTime;
	float startTime;

	// animation
	float frameDuration;
	float textureFrames;

	Explosion(ExplosionShader* shader);
	~Explosion() {};

	void Init(float elapsedTime, float scale, glm::vec3 position);
	void Draw(const glm::mat4& view, const glm::mat4& projection);
};

