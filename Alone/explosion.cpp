#include "explosion.h"
Explosion::Explosion(ExplosionShader* shader) {
	Explosion::shader = shader;
	frameDuration = 0.05f;
	textureFrames = 16;
}

void Explosion::Init(float elapsedTime, float scale, glm::vec3 position) {
	startTime	= elapsedTime;
	currentTime	= startTime;
	direction	= glm::vec3(0.0f, 0.0f, 1.0f);
	
	Explosion::scale	= scale;
	Explosion::position = position;
}

void Explosion::Draw(const glm::mat4& view, const glm::mat4& projection) {
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(shader->program);

	// take rotation part of the view transform
	glm::mat4 billboardRotationMatrix = glm::mat4(
		view[0],
		view[1],
		view[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	// inverse view rotation
	billboardRotationMatrix = glm::transpose(billboardRotationMatrix);

	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position);
	matrix = glm::scale(matrix, glm::vec3(scale));
	matrix = matrix * billboardRotationMatrix; // make billboard to face the camera

	glm::mat4 PVM = projection * view * matrix;
	glUniformMatrix4fv(shader->PVMLocation, 1, GL_FALSE, glm::value_ptr(PVM));	glUniformMatrix4fv(shader->viewLocation, 1, GL_FALSE, glm::value_ptr(view));   // view
	glUniform1f(shader->timeLocation, currentTime - startTime);
	glUniform1i(shader->textureSamplerLocation, 0);
	glUniform1f(shader->frameDurationLocation, frameDuration);

	glBindVertexArray(shader->vao);
	glBindTexture(GL_TEXTURE_2D, shader->texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glUseProgram(0);

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	return;
}
