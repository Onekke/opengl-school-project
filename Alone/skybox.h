#pragma once
#include "pgr.h"
#include <iostream>

class Skybox
{
	// buffers
	GLuint vbo;
	GLuint vao;

	// shader program
	GLuint program;

	// vertex attributes locations
	GLint  screenCoordsLocation;

	// uniforms locations
	GLint inversePVLocation;
	GLint textureSamplerLocation;
	GLint fogLocation;

	// texture
	GLuint texture;

public:
	Skybox(const char* filename);
	~Skybox() {};

	void SetFog(float fog);
	void Draw(const glm::mat4& view, const glm::mat4& projection);

private:
	void InitShaders();
	void InitGeometry();
	void LoadTexture(const char* filename);
};