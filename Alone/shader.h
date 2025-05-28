#pragma once
#include "pgr.h"

class Shader
{
public:
	// shader program id
	GLuint	program;

	// vertex attributes locations
	GLint	positionLocation;
	GLint	normalLocation;
	GLint	textureCoordsLocation;

private:
	// vertex uniforms locations
	GLint	viewLocation;
	GLint	modelLocation;
	GLint	projectionLocation;

	// fragmet uniform locations
	GLint	timeLocation;
	GLint	fogLocation;

	// material 
	GLint	materialDiffuseLocation;
	GLint	materialAmbientLocation;
	GLint	materialSpecularLocation;
	GLint	materialShininessLocation;

	// texture
	GLint	useTextureLocation;
	GLint	textureDiffuseLocation;
	GLint	textureSpecularLocation;

	// directional light
	GLint	directionalDirectionLocation;
	GLint	directionalAmbientLocation;
	GLint	directionalDiffuseLocation;
	GLint	directionalSpecularLocation;

	// point light
	GLint	pointTurnedOnLocation;
	GLint	pointPositionLocation;
	GLint	pointConstantLocation;
	GLint	pointLinearLocation;
	GLint	pointQuadraticLocation;
	GLint	pointAmbientLocation;
	GLint	pointDiffuseLocation;
	GLint	pointSpecularLocation;

	// spot light
	GLint	spotTurnedOnLocation;
	GLint	spotPositionLocation;
	GLint	spotDirectionLocation;
	GLint	spotCutOffLocation;
	GLint	spotOuterCutOffLocation;
	GLint	spotConstantLocation;
	GLint	spotLinearLocation;
	GLint	spotQuadraticLocation;
	GLint	spotAmbientLocation;
	GLint	spotDiffuseLocation;
	GLint	spotSpecularLocation;

	// camera
	GLint	viewPositionLocation;

public:
	Shader(const char* vert, const char* frag);
	~Shader() {};

	void SetTransformUniforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
	void SetMaterialUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess);
	void SetTextureUniforms(GLuint diffuse, GLuint specular);
	void SetDirectionalLightUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& direction);
	void SetPointLightUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& position, float constant, float linear, float quadratic);
	void SetSpotLightUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& direction, const glm::vec3& position, float constant, float linear, float quadratic, float cutOff, float outerCutOff);
	void SetViewPosition(glm::vec3 position);
	void UpdateTime(float time);
	void SetFog(float fog);

private:
	void Init(const char* vert, const char* frag);
};