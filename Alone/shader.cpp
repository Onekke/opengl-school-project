#include "shader.h"
Shader::Shader(const char* vert, const char* frag) {
	Init(vert, frag);
}

void Shader::SetTransformUniforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	CHECK_GL_ERROR();
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
	CHECK_GL_ERROR();
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
	CHECK_GL_ERROR();
}

void Shader::SetMaterialUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess) {
	glUniform3fv(materialDiffuseLocation, 1, glm::value_ptr(diffuse));
	glUniform3fv(materialAmbientLocation, 1, glm::value_ptr(ambient));
	glUniform3fv(materialSpecularLocation, 1, glm::value_ptr(specular));
	glUniform1f(materialShininessLocation, shininess);
	CHECK_GL_ERROR();

	// set texture, point and spot light to false for now
	glUniform1i(useTextureLocation, 0);
	glUniform1i(pointTurnedOnLocation, 0);
	glUniform1i(spotTurnedOnLocation, 0);
	CHECK_GL_ERROR();
}

void Shader::SetTextureUniforms(GLuint diffuse, GLuint specular) {
	glUniform1i(useTextureLocation, 1);

	glUniform1i(textureDiffuseLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse);
	CHECK_GL_ERROR();

	glUniform1i(textureSpecularLocation, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular);
	CHECK_GL_ERROR();
}

void Shader::SetDirectionalLightUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& direction) {
	glUseProgram(program);
	glUniform3fv(directionalDirectionLocation, 1, glm::value_ptr(direction));
	glUniform3fv(directionalAmbientLocation, 1, glm::value_ptr(ambient));
	glUniform3fv(directionalDiffuseLocation, 1, glm::value_ptr(diffuse));
	glUniform3fv(directionalSpecularLocation, 1, glm::value_ptr(specular));
	glUseProgram(0);
}

void Shader::SetPointLightUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& position, float constant, float linear, float quadratic) {
	glUseProgram(program);
	glUniform1i(pointTurnedOnLocation, 1);

	glUniform3fv(pointPositionLocation, 1, glm::value_ptr(position));
	glUniform1f(pointConstantLocation, constant);
	glUniform1f(pointLinearLocation, linear);
	glUniform1f(pointQuadraticLocation, quadratic);
	glUniform3fv(pointAmbientLocation, 1, glm::value_ptr(ambient));
	glUniform3fv(pointDiffuseLocation, 1, glm::value_ptr(diffuse));
	glUniform3fv(pointSpecularLocation, 1, glm::value_ptr(specular));
	glUseProgram(0);
}

void Shader::SetSpotLightUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& direction, const glm::vec3& position, float constant, float linear, float quadratic, float cutOff, float outerCutOff) {
	glUseProgram(program);
	glUniform1i(spotTurnedOnLocation, 1);

	glUniform3fv(spotPositionLocation, 1, glm::value_ptr(position));
	glUniform3fv(spotDirectionLocation, 1, glm::value_ptr(direction));
	glUniform1f(spotCutOffLocation, cutOff);
	glUniform1f(spotOuterCutOffLocation, outerCutOff);
	glUniform1f(spotConstantLocation, constant);
	glUniform1f(spotLinearLocation, linear);
	glUniform1f(spotQuadraticLocation, quadratic);
	glUniform3fv(spotAmbientLocation, 1, glm::value_ptr(ambient));
	glUniform3fv(spotDiffuseLocation, 1, glm::value_ptr(diffuse));
	glUniform3fv(spotSpecularLocation, 1, glm::value_ptr(specular));
	glUseProgram(0);
}

void Shader::SetViewPosition(glm::vec3 position) {
	glUseProgram(program);
	glUniform3fv(viewPositionLocation, 1, glm::value_ptr(position));
	glUseProgram(0);
}

void Shader::UpdateTime(float time) {
	glUseProgram(program);
	glUniform1f(timeLocation, time);
	glUseProgram(0);
}

void Shader::SetFog(float fog) {
	glUseProgram(program);
	glUniform1f(fogLocation, fog);
	glUseProgram(0);
}

void Shader::Init(const char* vert, const char* frag) {
	std::vector<GLuint> shaders;

	// load and compile shader for lighting (lights & materials)

	// push vertex shader and fragment shader
	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, vert));
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, frag));

	// create the program with two shaders
	program = pgr::createProgram(shaders);

	// get vertex attributes locations
	positionLocation = glGetAttribLocation(program, "position");
	normalLocation = glGetAttribLocation(program, "normal");
	textureCoordsLocation = glGetAttribLocation(program, "textureCoords");
	CHECK_GL_ERROR();

	// get vertex uniforms locations
	viewLocation = glGetUniformLocation(program, "view");
	modelLocation = glGetUniformLocation(program, "model");
	projectionLocation = glGetUniformLocation(program, "projection");
	timeLocation	   = glGetUniformLocation(program, "time");
	fogLocation = glGetUniformLocation(program, "fog");
	CHECK_GL_ERROR();

	// get fragment uniforms locations
	// texture
	useTextureLocation = glGetUniformLocation(program, "textureMaterial.useTexture");
	textureDiffuseLocation = glGetUniformLocation(program, "textureMaterial.diffuse");
	textureSpecularLocation = glGetUniformLocation(program, "textureMaterial.specular");
	CHECK_GL_ERROR();

	// material
	materialAmbientLocation = glGetUniformLocation(program, "material.ambient");
	materialDiffuseLocation = glGetUniformLocation(program, "material.diffuse");
	materialSpecularLocation = glGetUniformLocation(program, "material.specular");
	materialShininessLocation = glGetUniformLocation(program, "material.shininess");
	CHECK_GL_ERROR();

	// directional light
	directionalDirectionLocation = glGetUniformLocation(program, "directionalLight.direction");
	directionalAmbientLocation = glGetUniformLocation(program, "directionalLight.ambient");
	directionalDiffuseLocation = glGetUniformLocation(program, "directionalLight.diffuse");
	directionalSpecularLocation = glGetUniformLocation(program, "directionalLight.specular");
	CHECK_GL_ERROR();

	// point light
	pointTurnedOnLocation = glGetUniformLocation(program, "pointLight.turnedOn");
	pointPositionLocation = glGetUniformLocation(program, "pointLight.position");
	pointConstantLocation = glGetUniformLocation(program, "pointLight.constant");
	pointLinearLocation = glGetUniformLocation(program, "pointLight.linear");
	pointQuadraticLocation = glGetUniformLocation(program, "pointLight.quadratic");
	pointAmbientLocation = glGetUniformLocation(program, "pointLight.ambient");
	pointDiffuseLocation = glGetUniformLocation(program, "pointLight.diffuse");
	pointSpecularLocation = glGetUniformLocation(program, "pointLight.specular");
	CHECK_GL_ERROR();

	// spot light
	spotTurnedOnLocation = glGetUniformLocation(program, "spotLight.turnedOn");
	spotPositionLocation = glGetUniformLocation(program, "spotLight.position");
	spotDirectionLocation = glGetUniformLocation(program, "spotLight.direction");
	spotCutOffLocation = glGetUniformLocation(program, "spotLight.cutOff");
	spotOuterCutOffLocation = glGetUniformLocation(program, "spotLight.outerCutOff");
	spotConstantLocation = glGetUniformLocation(program, "spotLight.constant");
	spotLinearLocation = glGetUniformLocation(program, "spotLight.linear");
	spotQuadraticLocation = glGetUniformLocation(program, "spotLight.quadratic");
	spotAmbientLocation = glGetUniformLocation(program, "spotLight.ambient");
	spotDiffuseLocation = glGetUniformLocation(program, "spotLight.diffuse");
	spotSpecularLocation = glGetUniformLocation(program, "spotLight.specular");
	CHECK_GL_ERROR();

	// camera
	viewPositionLocation = glGetUniformLocation(program, "viewPosition");

	// clear stored shaders
	shaders.clear();
}