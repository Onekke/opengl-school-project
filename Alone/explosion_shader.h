#pragma once
#include "pgr.h"

class ExplosionShader
{
public:
	// buffers
	GLuint vbo;
	GLuint vao;

	// shader program
	GLuint program;

	// vertex attributes locations
	GLint positionLocation;
	GLint textureCoordsLocation;

	// uniforms locations
	GLint PVMLocation;
	GLint viewLocation;
	GLint timeLocation;
	GLint textureSamplerLocation;
	GLint frameDurationLocation;

	// texture
	GLuint texture;

	ExplosionShader(const char* filename);

private:
	void InitShaders();
	void InitGeometry(const char* filename);
};