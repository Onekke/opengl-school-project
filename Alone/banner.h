#pragma once
#include "pgr.h"
class Banner
{
	// buffers
	GLuint vbo;
	GLuint vao;

	GLuint program;   
 
	// vertex attributes locations
	GLint posLocation;
	GLint texCoordLocation;

	// uniforms locations
	GLint PVMmatrixLocation; 
	GLint timeLocation;      
	GLint texSamplerLocation;

	glm::vec3 position;
	glm::vec3 direction;
	float     speed;
	float     scale;

	GLuint  texture;

	unsigned int  numTriangles;

public:
	float currentTime;
	float startTime;

	Banner(float elapsedTime, const char* filename);
	~Banner() {};

	void Draw(const glm::mat4& view, const glm::mat4& projection);
	void Update(const glm::vec3& position);
private:
	void InitShader();
	void InitGeometry(const char* filename);
};