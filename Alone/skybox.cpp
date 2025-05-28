#include "skybox.h"

const float vertices[] = { -1.0f,  1.0f, -1.0f,
						   -1.0f, -1.0f, -1.0f,
							1.0f, -1.0f, -1.0f,
							1.0f, -1.0f, -1.0f,
							1.0f,  1.0f, -1.0f,
						   -1.0f,  1.0f, -1.0f,

						   -1.0f, -1.0f,  1.0f,
						   -1.0f, -1.0f, -1.0f,
						   -1.0f,  1.0f, -1.0f,
						   -1.0f,  1.0f, -1.0f,
						   -1.0f,  1.0f,  1.0f,
						   -1.0f, -1.0f,  1.0f,

							1.0f, -1.0f, -1.0f,
							1.0f, -1.0f,  1.0f,
							1.0f,  1.0f,  1.0f,
							1.0f,  1.0f,  1.0f,
							1.0f,  1.0f, -1.0f,
							1.0f, -1.0f, -1.0f,

						   -1.0f, -1.0f,  1.0f,
						   -1.0f,  1.0f,  1.0f,
							1.0f,  1.0f,  1.0f,
							1.0f,  1.0f,  1.0f,
							1.0f, -1.0f,  1.0f,
						   -1.0f, -1.0f,  1.0f,

						   -1.0f,  1.0f, -1.0f,
							1.0f,  1.0f, -1.0f,
							1.0f,  1.0f,  1.0f,
							1.0f,  1.0f,  1.0f,
						   -1.0f,  1.0f,  1.0f,
						   -1.0f,  1.0f, -1.0f,

						   -1.0f, -1.0f, -1.0f,
						   -1.0f, -1.0f,  1.0f,
							1.0f, -1.0f, -1.0f,
							1.0f, -1.0f, -1.0f,
						   -1.0f, -1.0f,  1.0f,
							1.0f, -1.0f,  1.0f };

#include "skybox.h"

Skybox::Skybox(const char* filename) {
	InitShaders();
	InitGeometry();
	LoadTexture(filename);
}

void Skybox::Draw(const glm::mat4& view, const glm::mat4& projection) {
	glDepthMask(GL_FALSE);	// to set skybox to be always drawn on the background
	glUseProgram(program);

	// compose transformation matrix
	glm::mat4 inversePV = glm::inverse(projection * glm::mat4(glm::mat3(view)));

	// set uniforms
	glUniformMatrix4fv(Skybox::inversePVLocation, 1, GL_FALSE, glm::value_ptr(inversePV));
	glUniform1i(textureSamplerLocation, 0);

	// draw skybox
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Skybox::SetFog(float fog) {
	glUseProgram(program);
	glUniform1f(fogLocation, fog);
	glUseProgram(0);
}

void Skybox::InitShaders() {
	std::vector<GLuint> shaders;

	// push vertex and fragment shader
	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "skybox.vert"));
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "skybox.frag"));

	// create a program with vertex and fragment shader
	program = pgr::createProgram(shaders);

	// get attributes locations
	screenCoordsLocation = glGetAttribLocation(program, "screenCoords");

	// get uniforms locations
	inversePVLocation = glGetUniformLocation(program, "inversePV");
	textureSamplerLocation = glGetUniformLocation(program, "textureSampler");
	fogLocation = glGetUniformLocation(program, "fog");
}

void Skybox::InitGeometry() {
	// vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// set screen coords
	glUseProgram(program);
	glEnableVertexAttribArray(screenCoordsLocation);
	glVertexAttribPointer(screenCoordsLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();
}

void Skybox::LoadTexture(const char* filename) {
	// activate single texure
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	// load texture images
	const char* suffixes[] = { "px", "nx", "py", "ny", "pz", "nz" };
	GLuint targets[] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
						 GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
						 GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

	for (int i = 0; i < 6; i++) {
		std::string textureName = std::string(filename) + "_" + suffixes[i] + ".png";
		std::cout << "Loading skybox texture: " << textureName << std::endl;
		if (!pgr::loadTexImage2D(textureName, targets[i])) {
			pgr::dieWithError("Skybox texture loading failed!");
		}
	}

	// set texture parameters
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERROR();
}