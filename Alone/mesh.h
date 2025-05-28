#pragma once
#include "pgr.h"
class Mesh
{
public:
	// buffers
	GLuint vbo;
	GLuint ebo;
	GLuint vao;

	// material
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float	  shininess;

	// texture
	bool   useTexture;
	GLuint textureDiffuse;
	GLuint textureSpecular;

	unsigned int  numTriangles;

public:
	Mesh() {};
	~Mesh() {};
};