#pragma once
#include <iostream>
#include "pgr.h"
#include "shader.h"
#include "mesh.h"
class Model
{
	Shader* shader;
	std::vector<Mesh*> modelGeometry;

public:
	Model(const std::string& fileName, Shader* shader);
	~Model() {};

	void Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
	bool LoadMesh(const std::string& fileName);
};